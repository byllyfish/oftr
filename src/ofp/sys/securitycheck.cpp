// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/sys/securitycheck.h"
#include "ofp/sys/connection.h"
#include "ofp/sys/identity.h"
#include "ofp/sys/memx509.h"

using namespace ofp;
using namespace ofp::sys;

static int tls_verify_callback(int preverified, X509_STORE_CTX *ctx);

static const char *MySSLVersionString(SSL *ssl) {
  switch (SSL_version(ssl)) {
    case SSL2_VERSION:
      return "SSL 2.0";
    case SSL3_VERSION:
      return "SSL 3.0";
    case TLS1_VERSION:
      return "TLS 1.0";
    case TLS1_1_VERSION:
      return "TLS 1.1";
    case TLS1_2_VERSION:
      return "TLS 1.2";
    case TLS1_3_VERSION:
      return "TLS 1.3";
    case DTLS1_VERSION:
      return "DTLS 1.0";
    case DTLS1_2_VERSION:
      return "DTLS 1.2";
    default:
      return "Unknown";
  }
}

template <>
void SecurityCheck::beforeHandshake<SSL>(Connection *conn, SSL *ssl,
                                         bool isClient) {
  assert(conn->flags() & Connection::kRequiresHandshake);
  assert(conn->flags() & ~Connection::kHandshakeDone);

  // Store connection pointer in SSL object.
  Identity::SetConnectionPtr(ssl, conn);

  // Retrieve identity pointer from SSL object.
  Identity *identity = Identity::GetIdentityPtr(ssl);

  // Set up the verify callback.
  SSL_set_verify(ssl, identity->peerVerifyMode(), tls_verify_callback);

  if (isClient) {
    // Check if there is a client session we can resume.
    SSL_SESSION *session = identity->findClientSession(conn->remoteEndpoint());
    if (session) {
      SSL_set_session(ssl, session);
    }
  }
}

template <>
void SecurityCheck::afterHandshake<SSL>(Connection *conn, SSL *ssl,
                                        std::error_code err) {
  assert(conn->flags() & Connection::kRequiresHandshake);
  assert(conn->flags() & ~Connection::kHandshakeDone);

  conn->setFlags(conn->flags() | Connection::kHandshakeDone);

  // Retrieve identity pointer from SSL object.
  Identity *identity = Identity::GetIdentityPtr(ssl);
  bool isClient = !SSL_is_server(ssl);

  UInt64 securityId = identity->securityId();
  UInt64 connId = conn->connectionId();

  const char *tlsVersion = MySSLVersionString(ssl);

  if (err) {
    log_error(tlsVersion, "handshake failed",
              std::make_pair("tlsid", securityId),
              std::make_pair("connid", connId), err);
    return;
  }

  const char *tlsCipherSpec = SSL_get_cipher_name(ssl);
  const bool sessionResumed = SSL_session_reused(ssl);
  const char *sessionStatus = sessionResumed ? "resumed" : "started";

  std::string sessionId;
  if (sessionResumed) {
    unsigned int idlen;
    const UInt8 *id = SSL_SESSION_get_id(SSL_get_session(ssl), &idlen);
    sessionId = RawDataToHex(id, idlen);
  }

  log_info(tlsVersion, "session", sessionStatus, tlsCipherSpec, sessionId,
           std::make_pair("tlsid", securityId),
           std::make_pair("connid", connId));

  // If this is a client handshake, save the client session.
  if (isClient) {
    identity->saveClientSession(conn->remoteEndpoint(), SSL_get1_session(ssl));
  }
}

template <>
void SecurityCheck::beforeClose<SSL>(Connection *conn, SSL *ssl) {
  auto flags = conn->flags();
  if ((flags & Connection::kHandshakeDone) &&
      !(flags & Connection::kShutdownDone)) {
    Identity *identity = Identity::GetIdentityPtr(ssl);
    UInt64 securityId = identity->securityId();
    UInt64 connId = conn->connectionId();

    const char *tlsVersion = MySSLVersionString(ssl);

    log_warning(tlsVersion, "incomplete shutdown",
                std::make_pair("tlsid", securityId),
                std::make_pair("connid", connId));
  }
}

int tls_verify_callback(int preverified, X509_STORE_CTX *ctx) {
  log::fatal_if_null(ctx);

  // Retrieve pointer to SSL object from the X509_STORE_CTX. Then, retrieve our
  // stored pointers to the identity and connection objects from the SSL object.

  SSL *ssl = static_cast<SSL *>(::X509_STORE_CTX_get_ex_data(
      ctx, ::SSL_get_ex_data_X509_STORE_CTX_idx()));
  log::fatal_if_null(ssl);

  Identity *identity = Identity::GetIdentityPtr(ssl);
  Connection *conn = Identity::GetConnectionPtr(ssl);

  UInt64 securityId = identity->securityId();
  UInt64 connId = conn->connectionId();

  // Retrieve error code and depth from X509_STORE_CTX.

  int error = X509_STORE_CTX_get_error(ctx);
  int depth = X509_STORE_CTX_get_error_depth(ctx);

  // Retrieve subject name of the current (possibly intermediate) certificate.
  // N.B. do not delete the certificate; we don't own it!

  MemX509 cert{X509_STORE_CTX_get_current_cert(ctx), false};
  log::fatal_if_null(cert.get());

  std::string subjectName = cert.subjectName();

  // Check status of preverification.

  if (!preverified || (error != X509_V_OK)) {
    // We failed pre-verification or there is a verify error.

    log_warning(
        "Certificate verify failed:", X509_verify_cert_error_string(error),
        std::make_pair("connid", connId));

    log_warning("Peer certificate", depth, subjectName,
                std::make_pair("tlsid", securityId),
                std::make_pair("connid", connId), '\n', cert.toString());
    return 0;
  }

  assert(preverified != 0);

  // At this point, our certificate has passed OpenSSL's pre-verification. An
  // intermediate certificate will have a depth greater than zero.

  if (depth > 0) {
    log_debug("Verify peer chain:", depth, subjectName,
              std::make_pair("tlsid", securityId),
              std::make_pair("connid", connId));
    return 1;
  }

  log_info("Verify peer:", subjectName, std::make_pair("tlsid", securityId),
           std::make_pair("connid", connId));

  return 1;
}
