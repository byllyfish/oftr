// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/sys/dtls_adapter.h"

using namespace ofp::sys;

DTLS_Adapter::DTLS_Adapter(asio::ssl::context *context,
                           DeliverFunc sendCallback,
                           DeliverFunc receiveCallback, void *userData)
    : sendCallback_{sendCallback},
      receiveCallback_{receiveCallback},
      userData_{userData} {
  ssl_ = SSL_new(context->native_handle());
  log::fatal_if_null(ssl_, LOG_LINE());

  int rc = SSL_set_ssl_method(ssl_, DTLSv1_method());
  log::fatal_if_false(rc == 1, LOG_LINE());

  reading_ = BIO_new(BIO_s_mem());
  log::fatal_if_null(reading_, LOG_LINE());

  writing_ = BIO_new(BIO_s_mem());
  log::fatal_if_null(writing_, LOG_LINE());

  BIO_set_mem_eof_return(reading_, -1);
  BIO_set_mem_eof_return(writing_, -1);
  SSL_set_bio(ssl_, reading_, writing_);
}

DTLS_Adapter::~DTLS_Adapter() {
  SSL_free(ssl_);
  ssl_ = nullptr;
  reading_ = nullptr;
  writing_ = nullptr;
}

void DTLS_Adapter::connect() {
  // Initialize DTLS connection to client mode.
  SSL_set_connect_state(ssl_);

  // Get the ball rolling by sending an empty datagram.
  sendDatagram("", 0);
}

void DTLS_Adapter::accept() {
  // Enable DTLS cookie exchange.
  (void)SSL_set_options(ssl_, SSL_OP_COOKIE_EXCHANGE);

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
  int rc;

  rc = SSL_write(ssl_, datagram, static_cast<int>(length));
  log::debug("sendDatagram: SSL_write returned", rc);

  if (rc <= 0) {
    int err = SSL_get_error(ssl_, rc);

    switch (err) {
      case SSL_ERROR_WANT_READ:
        // We are waiting for some data to arrive before we can send
        // the next datagram. Add the datagram to a queue.
        if (length > 0) 
          enqueueDatagram(datagram, length);
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

  UInt8 outBuf[4000];
  rc = BIO_read(writing_, outBuf, sizeof(outBuf));
  log::debug("sendDatagram: BIO_read returned", rc);

  if (rc > 0) {
    sendCallback_(outBuf, Unsigned_cast(rc), userData_);
  }
}

void DTLS_Adapter::datagramReceived(const void *datagram, size_t length) {
  int rc;

  rc = BIO_write(reading_, datagram, static_cast<int>(length));
  log::debug("datagramReceived: BIO_write returned", rc,
             ByteRange{datagram, length});

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

  size_t wlen = BIO_ctrl_pending(writing_);
  log::debug("datagramReceived: BIO_ctrl_pending returned", wlen);

  if (wlen > 0) {
    UInt8 outBuf[4000];
    rc = BIO_read(writing_, outBuf, sizeof(outBuf));
    log::debug("datagramReceived: BIO_read returned", rc);

    if (rc > 0) {
      sendCallback_(outBuf, Unsigned_cast(rc), userData_);
    }
  }

  if (SSL_is_init_finished(ssl_) && !datagrams_.empty()) {
    flushDatagrams();
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
