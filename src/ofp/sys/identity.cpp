// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/sys/identity.h"
#include "ofp/sys/membio.h"
#include "ofp/sys/memx509.h"
#include "ofp/sys/connection.h"

using namespace ofp::sys;

// First call to SSL_CTX_get_ex_new_index works around an issue in ASIO where
// it uses SSL_CTX_get_app_data.

const int SSL_CTX_ASIO_PTR =
    SSL_CTX_get_ex_new_index(0, nullptr, nullptr, nullptr, nullptr);
const int SSL_CTX_IDENTITY_PTR =
    SSL_CTX_get_ex_new_index(0, nullptr, nullptr, nullptr, nullptr);

const int SSL_ASIO_PTR =
    SSL_get_ex_new_index(0, nullptr, nullptr, nullptr, nullptr);
const int SSL_CONNECTION_PTR =
    SSL_get_ex_new_index(0, nullptr, nullptr, nullptr, nullptr);

static Identity *GetIdentityPtr(SSL_CTX *ctx) {
  return ofp::log::fatal_if_null(
      static_cast<Identity *>(SSL_CTX_get_ex_data(ctx, SSL_CTX_IDENTITY_PTR)));
}

static void SetIdentityPtr(SSL_CTX *ctx, Identity *identity) {
  SSL_CTX_set_ex_data(ctx, SSL_CTX_IDENTITY_PTR, identity);
}

static Connection *GetConnectionPtr(SSL *ssl) {
  return ofp::log::fatal_if_null(
      static_cast<Connection *>(SSL_get_ex_data(ssl, SSL_CONNECTION_PTR)));
}

static void SetConnectionPtr(SSL *ssl, Connection *conn) {
  SSL_set_ex_data(ssl, SSL_CONNECTION_PTR, conn);
}

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
    case DTLS1_VERSION:
      return "DTLS 1.0";
    default:
      return "Unknown";
  }
}

static bool IsDTLS(SSL *ssl) {
  switch (SSL_version(ssl)) {
    case DTLS1_VERSION:
      return true;
    default:
      return false;
  }
}

constexpr asio::ssl::context_base::method defaultMethod() {
#if defined(SSL_TXT_TLSV1_2)
  return asio::ssl::context_base::tlsv12;
#elif defined(SSL_TXT_TLSV1_1)
  return asio::ssl::context_base::tlsv11;
#else
  return asio::ssl::context_base::tlsv1;
#endif
}

Identity::Identity(const std::string &certData,
                   const std::string &keyPassphrase,
                   const std::string &verifyData, std::error_code &error)
    : context_{defaultMethod()} {
  error = configureContext();
  if (error) return;

  error = loadCertificateChain(certData);
  if (error) return;

  error = loadPrivateKey(certData, keyPassphrase);
  if (error) return;

  error = loadVerifier(verifyData);
  if (error) return;

  error = prepareVerifier();
  if (error) return;

  error = prepareCookies();
  if (error) return;

  // Save subject name of the certificate.
  MemX509 cert{certData};
  subjectName_ = cert.subjectName();
}

Identity::~Identity() {
  for (auto &item : clientSessions_) {
    SSL_SESSION_free(item.second);
  }
}

SSL_SESSION *Identity::findClientSession(const IPv6Endpoint &remoteEndpt) {
  auto iter = clientSessions_.find(remoteEndpt);
  if (iter == clientSessions_.end()) return nullptr;

  return iter->second;
}

void Identity::saveClientSession(const IPv6Endpoint &remoteEndpt,
                                 SSL_SESSION *session) {
  if (!remoteEndpt.valid()) {
    log::warning("Identity::saveClientSession: invalid endpoint detected");
    return;
  }

  auto ptr = &clientSessions_[remoteEndpt];
  auto prevSession = *ptr;
  *ptr = session;

  if (prevSession) {
    SSL_SESSION_free(prevSession);
  }
}

std::error_code Identity::configureContext() {
  std::error_code err;

  (void)SSL_CTX_clear_options(context_.native_handle(),
                              SSL_OP_LEGACY_SERVER_CONNECT);
  auto options = SSL_CTX_set_options(context_.native_handle(),
                                     SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);
  log::debug("ConfigureContext: options", options);

  // context_.set_options(asio::ssl::context::no_sslv2 |
  //                         asio::ssl::context::no_sslv3 |
  //                         asio::ssl::context::no_compression,
  //                     err);

  // context_.clear_options();

  uint8_t id[] = "ofpx";
  SSL_CTX_set_session_id_context(context_.native_handle(), id, sizeof(id) - 1);
  SSL_CTX_set_timeout(context_.native_handle(), 60 * 5);
  SSL_CTX_set_session_cache_mode(context_.native_handle(),
                                 SSL_SESS_CACHE_SERVER);

  // Allow for retrieving this Identity object given just an SSL context.
  SetIdentityPtr(context_.native_handle(), this);

  // Install msg callback to log handshake messages.
  SSL_CTX_set_msg_callback(context_.native_handle(), openssl_msg_callback);

  return err;
}

/// Load certificate from a PEM file.
std::error_code Identity::loadCertificateChain(const std::string &certData) {
  std::error_code err;

  asio::const_buffer certBuffer{certData.data(), certData.size()};
  context_.use_certificate_chain(certBuffer, err);

  if (err) {
    log::error("Failed to load certificate", err);
  }

  return err;
}

/// Load private key from a PEM file.
std::error_code Identity::loadPrivateKey(const std::string &keyData,
                                         const std::string &passphrase) {
  std::error_code err;

  // Set the context's password callback to return the given passphrase.
  context_.set_password_callback(
      [&passphrase](size_t maxLength,
                    asio::ssl::context::password_purpose purpose)
          -> std::string {
        return (purpose == asio::ssl::context::for_reading) ? passphrase : "";
      },
      err);

  if (err) {
    log::warning("loadPrivateKey: Error returned from set_password_callback",
                 err);
  }

  asio::const_buffer keyBuffer{keyData.data(), keyData.size()};
  context_.use_private_key(keyBuffer, asio::ssl::context::pem, err);

  if (err) {
    log::error("Failed to load private key", err);
  }

  // After loading the key, reset the context's password callback.
  std::error_code ignore;
  context_.set_password_callback(
      [](size_t maxLength, asio::ssl::context::password_purpose purpose)
          -> std::string { return ""; },
      ignore);

  if (!err) {
    // Make sure the key matches the certificate.
    int valid = SSL_CTX_check_private_key(context_.native_handle());
    if (!valid) {
      log::warning("loadPrivateKey: private key does not match certificate");
    }
  }

  return err;
}

std::error_code Identity::loadVerifier(const std::string &verifyData) {
  std::error_code err;

  asio::const_buffer verifyBuffer{verifyData.data(), verifyData.size()};
  context_.add_certificate_authority(verifyBuffer, err);

  if (err) {
    log::error("Failed to load verifier:", err);
  } else {
    // Add CA certificate name to our client CA list.
    err = addClientCA(verifyData);
  }

  return err;
}

std::error_code Identity::addClientCA(const std::string &verifyData) {
  ::ERR_clear_error();

  MemX509 cert{verifyData};
  if (cert) {
    if (::SSL_CTX_add_client_CA(context_.native_handle(), cert) == 1) {
      return std::error_code{};
    }
  }

  return std::error_code(static_cast<int>(::ERR_get_error()),
                         asio::error::get_ssl_category());
}

std::error_code Identity::prepareVerifier() {
  std::error_code err;

  // Set the verify mode to "verify the peer" and "fail verification if peer
  // certificate is not present". In a client handshake, the verify_fail_if
  // no_cert option is ignored -- I'm just setting the verify mode here.

  context_.set_verify_mode(
      asio::ssl::verify_peer | asio::ssl::verify_fail_if_no_peer_cert, err);

  // By default, set up the context to REJECT all certificates. The TLS
  // connection
  // object should have its verify_callback set instead.

  context_.set_verify_callback(
      [this](bool preverified, asio::ssl::verify_context &ctx) -> bool {
        log::warning(
            "Context TLS verifier is rejecting all certificates - you need to "
            "override!");
        return false;
      },
      err);

  return err;
}

std::error_code Identity::prepareCookies() {
  SSL_CTX_set_cookie_generate_cb(context_.native_handle(),
                                 openssl_cookie_generate_callback);
  SSL_CTX_set_cookie_verify_cb(context_.native_handle(),
                               openssl_cookie_verify_callback);

  return {};
}

#if 0
bool Identity::verifyPeer(UInt64 connId, UInt64 securityId, bool preverified,
                          X509_STORE_CTX *ctx) {
  int error = X509_STORE_CTX_get_error(ctx.native_handle());
  int depth = X509_STORE_CTX_get_error_depth(ctx.native_handle());

  MemX509 cert{X509_STORE_CTX_get_current_cert(ctx.native_handle())};
  log::fatal_if_null(cert.ptr());

  std::string subjectName = cert.subjectName();

  if (!preverified || (error != X509_V_OK)) {
    // We failed pre-verification or there is a verify error.
    log::warning("Certificate verify failed:",
                 X509_verify_cert_error_string(error),
                 std::make_pair("connid", connId));
    log::warning("Peer certificate", depth, subjectName,
                 std::make_pair("tlsid", securityId),
                 std::make_pair("connid", connId), '\n', cert.toString());
    return false;
  }

  if (depth > 0) {
    log::debug("Verify peer chain:", depth, subjectName,
               std::make_pair("tlsid", securityId),
               std::make_pair("connid", connId));
    return preverified;
  }

  assert(depth == 0);
  log::info("Verify peer:", subjectName, std::make_pair("tlsid", securityId),
            std::make_pair("connid", connId));

  return true;
}
#endif  // 0

//-------------------------------//
// b e f o r e H a n d s h a k e //
//-------------------------------//

template <>
void Identity::beforeHandshake<SSL>(Connection *conn, SSL *ssl, bool isClient) {
  assert(conn->flags() & Connection::kRequiresHandshake);
  assert(conn->flags() & ~Connection::kHandshakeDone);

  // Store connection pointer in SSL object.
  SetConnectionPtr(ssl, conn);

  // Retrieve identity pointer from SSL_CTX object.
  Identity *identity = GetIdentityPtr(SSL_get_SSL_CTX(ssl));

  // Set up the verify callback.
  int verifyMode = SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
  SSL_set_verify(ssl, verifyMode, openssl_verify_callback);

#if 0
  sock.set_verify_callback(
      [connId, securityId](bool preverified, asio::ssl::verify_context &ctx)
          -> bool { return verifyPeer(connId, securityId, preverified, ctx); },
      err);

  if (err) {
    log::error("Failed to specify TLS verifier callback", err);
  }
#endif  // 0

  if (isClient && !IsDTLS(ssl)) {
    // Check if there is a client session we can resume. This is a static method
    // so we have to retrieve the Identity object from the SSL_CTX.

    SSL_SESSION *session = identity->findClientSession(conn->remoteEndpoint());
    if (session) {
      SSL_set_session(ssl, session);
    }
  }
}

template <>
void Identity::afterHandshake<SSL>(Connection *conn, SSL *ssl,
                                   std::error_code err) {
  assert(conn->flags() & Connection::kRequiresHandshake);
  assert(conn->flags() & ~Connection::kHandshakeDone);

  conn->setFlags(conn->flags() | Connection::kHandshakeDone);

  // Retrieve identity pointer from SSL_CTX object.
  Identity *identity = GetIdentityPtr(SSL_get_SSL_CTX(ssl));
  bool isClient = !ssl->server;

  UInt64 securityId = identity->securityId();
  UInt64 connId = conn->connectionId();

  const char *tlsVersion = MySSLVersionString(ssl);

  if (err) {
    log::error(tlsVersion, "handshake failed",
               std::make_pair("tlsid", securityId),
               std::make_pair("connid", connId), err);
    return;
  }

  assert(SSL_get_verify_result(ssl) == X509_V_OK);

  const char *tlsCipherSpec = SSL_get_cipher_name(ssl);
  const bool sessionResumed = SSL_session_reused(ssl);
  const char *sessionStatus = sessionResumed ? "resumed" : "started";

  std::string sessionId;
  if (sessionResumed) {
    unsigned int idlen;
    const UInt8 *id = SSL_SESSION_get_id(SSL_get_session(ssl), &idlen);
    sessionId = RawDataToHex(id, idlen);
  }

  log::info(tlsVersion, "session", sessionStatus, tlsCipherSpec, sessionId,
            std::make_pair("tlsid", securityId),
            std::make_pair("connid", connId));

  // If this is a client handshake, save the client session.
  if (isClient && !IsDTLS(ssl)) {
    identity->saveClientSession(conn->remoteEndpoint(), SSL_get1_session(ssl));
  }
}

template <>
void Identity::beforeClose<SSL>(Connection *conn, SSL *ssl) {
  auto flags = conn->flags();
  if ((flags & Connection::kHandshakeDone) && !(flags & Connection::kShutdownDone)) {
    Identity *identity = GetIdentityPtr(SSL_get_SSL_CTX(ssl));
    UInt64 securityId = identity->securityId();
    UInt64 connId = conn->connectionId();

    const char *tlsVersion = MySSLVersionString(ssl);

    log::warning(tlsVersion, "incomplete shutdown", std::make_pair("tlsid", securityId),
               std::make_pair("connid", connId));
  }
}

//-----------------------------------------------//
// o p e n s s l _ v e r i f y _ c a l l b a c k //
//-----------------------------------------------//

int Identity::openssl_verify_callback(int preverified, X509_STORE_CTX *ctx) {
  log::fatal_if_null(ctx);

  // Retrieve pointer to SSL object from the X509_STORE_CTX. Then, retrieve our
  // stored pointers to the identity and connection objects from the SSL object.

  SSL *ssl = static_cast<SSL *>(::X509_STORE_CTX_get_ex_data(
      ctx, ::SSL_get_ex_data_X509_STORE_CTX_idx()));
  log::fatal_if_null(ssl);

  Identity *identity = GetIdentityPtr(SSL_get_SSL_CTX(ssl));
  Connection *conn = GetConnectionPtr(ssl);

  UInt64 securityId = identity->securityId();
  UInt64 connId = conn->connectionId();

  // Retrieve error code and depth from X509_STORE_CTX.

  int error = X509_STORE_CTX_get_error(ctx);
  int depth = X509_STORE_CTX_get_error_depth(ctx);

  // Retrieve subject name of the current (possibly intermediate) certificate.

  MemX509 cert{X509_STORE_CTX_get_current_cert(ctx)};
  log::fatal_if_null(cert.ptr());
  std::string subjectName = cert.subjectName();

  // Check status of preverification.

  if (!preverified || (error != X509_V_OK)) {
    // We failed pre-verification or there is a verify error.

    log::warning("Certificate verify failed:",
                 X509_verify_cert_error_string(error),
                 std::make_pair("connid", connId));

    log::warning("Peer certificate", depth, subjectName,
                 std::make_pair("tlsid", securityId),
                 std::make_pair("connid", connId), '\n', cert.toString());
    return 0;
  }

  assert(preverified != 0);

  // At this point, our certificate has passed OpenSSL's pre-verification. An
  // intermediate certificate will have a depth greater than zero.

  if (depth > 0) {
    log::debug("Verify peer chain:", depth, subjectName,
               std::make_pair("tlsid", securityId),
               std::make_pair("connid", connId));
    return 1;
  }

  log::info("Verify peer:", subjectName, std::make_pair("tlsid", securityId),
            std::make_pair("connid", connId));

  return 1;
}

int Identity::openssl_cookie_generate_callback(SSL *ssl, uint8_t *cookie,
                                               size_t *cookie_len) {
  Connection *conn = GetConnectionPtr(ssl);
  log::debug("openssl_cookie_generate_callback", std::make_pair("connid", conn->connectionId()));
  memset(cookie, 0, 16);
  *cookie_len = 16;
  return 1;
}

int Identity::openssl_cookie_verify_callback(SSL *ssl, const uint8_t *cookie,
                                             size_t cookie_len) {
  Connection *conn = GetConnectionPtr(ssl);
  log::debug("openssl_cookie_verify_callback", std::make_pair("connid", conn->connectionId()));
  return 1;
}

void Identity::openssl_msg_callback(int write_p, int version, int content_type, const void *buf, size_t len, SSL *ssl, void *arg) {
  Connection *conn = GetConnectionPtr(ssl);
  const char *readWrite = write_p ? "TLS write" : "TLS read";
  log::debug(readWrite, "version", version, "type", content_type, "len", len, RawDataToHex(buf, len), std::make_pair("connid", conn->connectionId()));
}
