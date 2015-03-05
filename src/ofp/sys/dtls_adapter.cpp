// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/sys/dtls_adapter.h"
#include "ofp/sys/dtls_utils.h"

using namespace ofp::sys;

DTLS_Adapter::DTLS_Adapter(SSL_CTX *ctx, DeliverFunc sendCallback,
                           DeliverFunc receiveCallback, void *userData)
    : sendCallback_{sendCallback},
      receiveCallback_{receiveCallback},
      userData_{userData} {
  ssl_ = SSL_new(ctx);
  log::fatal_if_null(ssl_, LOG_LINE());

  // TODO(bfish): Figure out why DTLSv1_2_method() doesn't work? DTLS Handshake
  // doesn't complete normally!

  // int rc = SSL_set_ssl_method(ssl_, DTLSv1_method());
  // log::fatal_if_false(rc == 1, LOG_LINE());

  ::SSL_set_mode(ssl_, SSL_MODE_ENABLE_PARTIAL_WRITE);
  ::SSL_set_mode(ssl_, SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);

  // By default, let OpenSSL guess the MTU.
  // setMTU(499);

  BIO *int_bio = nullptr;
  BIO_new_bio_pair(&int_bio, 0, &bio_, 0);
  SSL_set_bio(ssl_, int_bio, int_bio);
}

DTLS_Adapter::~DTLS_Adapter() {
  BIO_free(bio_);
  bio_ = nullptr;

  SSL_free(ssl_);
  ssl_ = nullptr;
}

void DTLS_Adapter::setMTU(size_t mtu) {
  if (mtu > 0) {
    ::SSL_set_options(ssl_, SSL_OP_NO_QUERY_MTU);
    ::SSL_set_mtu(ssl_, static_cast<int32_t>(mtu));
  } else {
    ::SSL_clear_options(ssl_, SSL_OP_NO_QUERY_MTU);
  }
}

void DTLS_Adapter::connect() {
  // Initialize DTLS connection to client mode.
  SSL_set_connect_state(ssl_);

  // Get the ball rolling by sending an empty datagram.
  sendDatagram("", 0);
}

void DTLS_Adapter::accept() {
// Enable DTLS cookie exchange.
#if !defined(OPENSSL_IS_BORINGSSL)
  (void)SSL_set_options(ssl_, SSL_OP_COOKIE_EXCHANGE);
#endif  // !defined(OPENSSL_IS_BORINGSSL)

  // Initialize DTLS connection to server mode.
  SSL_set_accept_state(ssl_);
}

bool DTLS_Adapter::isHandshakeDone() const {
  log::debug("isHandshakeDone: state", SSL_state_string_long(ssl_),
             ERR_peek_last_error());
  struct timeval tv = {0, 0};
  if (DTLSv1_get_timeout(ssl_, &tv) == 1) {
    log::debug("isHandshakeDone: timeout", tv.tv_sec, tv.tv_usec);
  }
  return SSL_is_init_finished(ssl_);
}

void DTLS_Adapter::sendDatagram(const void *datagram, size_t length) {
  // Write the datagram through the SSL bio.
  int rc = SSL_write(ssl_, datagram, static_cast<int>(length));
  log::debug("sendDatagram: SSL_write returned", rc);

  if (rc <= 0) {
    int err = SSL_get_error(ssl_, rc);

    switch (err) {
      case SSL_ERROR_WANT_READ:
        // We are waiting for some data to arrive before we can send
        // the next datagram. Add the datagram to our queue so we can retry
        // it later.
        if (length > 0) enqueueDatagram(datagram, length);
        break;

      case SSL_ERROR_WANT_WRITE:
        // This should not happen. SSL_write should be able to put
        // any necessary outgoing data into the memory bio.
        log::warning("DTLS_Adapter::sendDatagram: SSL_ERROR_WANT_WRITE");
        break;

      case SSL_ERROR_SYSCALL:
        log::warning("DTLS_Adapter::sendDatagram: SSL_ERROR_SYSCALL");
        log::debug("DTLS_Adapter::sendDatagram: state",
                   SSL_state_string_long(ssl_), ERR_peek_last_error());
        break;

      default:
        log::warning("DTLS_Adapter::sendDatagram: unexpected error", err);
        break;
    }

    log::debug("sendDatagram: SSL_get_error returned", err, "state",
               SSL_state_string_long(ssl_));
  }

  writeOutput();
}

void DTLS_Adapter::datagramReceived(const void *datagram, size_t length) {
  int rc;

  log::debug("datagramReceived:", DTLS_PrintRecord(datagram, length));

  rc = BIO_write(bio_, datagram, static_cast<int>(length));
  // log::debug("datagramReceived: BIO_write returned", rc,
  //           ByteRange{datagram, length});

  UInt8 inBuf[4000];
  rc = SSL_read(ssl_, inBuf, sizeof(inBuf));
  log::debug("datagramReceived: SSL_read returned", rc);

  if (rc > 0) {
    receiveCallback_(inBuf, Unsigned_cast(rc), userData_);

  } else {
    int err = SSL_get_error(ssl_, rc);
    log::debug("datagramReceived: SSL_get_error returned", err, "state",
               SSL_state_string_long(ssl_), ERR_peek_last_error());
  }

  writeOutput();

  if (SSL_is_init_finished(ssl_) && !datagrams_.empty()) {
    log::debug("SSL_is_init_finished");
    flushDatagrams();
  }
}

void DTLS_Adapter::writeOutput() {
  while (BIO_pending(bio_) > 0) {
    UInt8 buf[1500];
    int rc = BIO_read(bio_, buf, 13);
    assert(rc == 13);

    size_t recordLen = (UInt32_cast(buf[11]) << 8) | UInt32_cast(buf[12]);
    assert(recordLen <= 1500 - 13);

    rc = BIO_read(bio_, &buf[13], static_cast<int>(recordLen));
    assert(Unsigned_cast(rc) == recordLen);

    log::debug("DTLS_Adapter:", DTLS_PrintRecord(buf, recordLen + 13));
    sendCallback_(buf, recordLen + 13, userData_);
  }
}

void DTLS_Adapter::enqueueDatagram(const void *data, size_t length) {
  datagrams_.emplace_back();
  datagrams_.back().write(data, length);
}

void DTLS_Adapter::flushDatagrams() {
  log::debug("DTLS_Adapter::flushDatagrams");
  while (!datagrams_.empty()) {
    Datagram &datagram = datagrams_.front();
    sendDatagram(datagram.data(), datagram.size());
    datagrams_.pop_front();
  }
}

void DTLS_Adapter::logOutput(const UInt8 *p, size_t length) {
  // Log DTLS records sent.

  while (length > 0) {
    size_t recordLen = DTLS_GetRecordLength(p, length);
    if (recordLen == 0) {
      break;
    }

    log::debug("DTLS_Adapter:", DTLS_PrintRecord(p, recordLen));

    assert(recordLen <= length);

    p += recordLen;
    length -= recordLen;
  }
}
