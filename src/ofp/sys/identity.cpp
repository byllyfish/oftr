// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/sys/identity.h"
#include "ofp/sys/connection.h"
#include "ofp/sys/membio.h"
#include "ofp/sys/memx509.h"

using namespace ofp;
using namespace ofp::sys;

// First call to SSL_CTX_get_ex_new_index works around an issue in ASIO where
// it uses SSL_CTX_get_app_data.

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

const int SSL_CTX_ASIO_PTR =
    SSL_CTX_get_ex_new_index(0, nullptr, nullptr, nullptr, nullptr);
const int SSL_CTX_IDENTITY_PTR =
    SSL_CTX_get_ex_new_index(0, nullptr, nullptr, nullptr, nullptr);

const int SSL_ASIO_PTR =
    SSL_get_ex_new_index(0, nullptr, nullptr, nullptr, nullptr);
const int SSL_CONNECTION_PTR =
    SSL_get_ex_new_index(0, nullptr, nullptr, nullptr, nullptr);

OFP_END_IGNORE_GLOBAL_CONSTRUCTOR

Identity *Identity::GetIdentityPtr(SSL_CTX *ctx) {
  return ofp::log::fatal_if_null(
      static_cast<Identity *>(SSL_CTX_get_ex_data(ctx, SSL_CTX_IDENTITY_PTR)));
}

void Identity::SetIdentityPtr(SSL_CTX *ctx, Identity *identity) {
  SSL_CTX_set_ex_data(ctx, SSL_CTX_IDENTITY_PTR, identity);
}

Connection *Identity::GetConnectionPtr(SSL *ssl) {
  return ofp::log::fatal_if_null(
      static_cast<Connection *>(SSL_get_ex_data(ssl, SSL_CONNECTION_PTR)));
}

void Identity::SetConnectionPtr(SSL *ssl, Connection *conn) {
  SSL_set_ex_data(ssl, SSL_CONNECTION_PTR, conn);
}

Identity::Identity(const std::string &certData, const std::string &privKey,
                   const std::string &verifyData, std::error_code &error)
    : tls_{asio::ssl::context_base::tlsv12},
      dtls_{::SSL_CTX_new(::DTLSv1_method()), ::SSL_CTX_free} {
  // Allow for retrieving this Identity object given just an SSL context.
  SetIdentityPtr(tls_.native_handle(), this);
  SetIdentityPtr(dtls_.get(), this);

  // Initialize the TLS context.
  error = initContext(tls_.native_handle(), certData, privKey, verifyData);
  if (error)
    return;

  // Initialize the DTLS context identically.
  error = initContext(dtls_.get(), certData, privKey, verifyData);
  if (error)
    return;

  // Save subject name of the certificate.
  MemX509 cert{certData};
  subjectName_ = cert.subjectName();
}

Identity::~Identity() {
#if IDENTITY_SESSIONS_ENABLED
  for (auto &item : clientSessions_) {
    SSL_SESSION_free(item.second);
  }
#endif  // IDENTITY_SESSIONS_ENABLED
}

SSL_SESSION *Identity::findClientSession(const IPv6Endpoint &remoteEndpt) {
#if IDENTITY_SESSIONS_ENABLED
  auto iter = clientSessions_.find(remoteEndpt);
  if (iter == clientSessions_.end())
    return nullptr;
  return iter->second;
#else
  return nullptr;
#endif  // !IDENTITY_SESSIONS_ENABLED
}

void Identity::saveClientSession(const IPv6Endpoint &remoteEndpt,
                                 SSL_SESSION *session) {
#if IDENTITY_SESSIONS_ENABLED
  if (!remoteEndpt.valid()) {
    log_warning("Identity::saveClientSession: invalid endpoint detected");
    return;
  }

  auto ptr = &clientSessions_[remoteEndpt];
  auto prevSession = *ptr;
  *ptr = session;

  if (prevSession) {
    SSL_SESSION_free(prevSession);
  }
#endif  // IDENTITY_SESSIONS_ENABLED
}

std::error_code Identity::initContext(SSL_CTX *ctx, const std::string &certData,
                                      const std::string &privKey,
                                      const std::string &verifyData) {
  prepareOptions(ctx);
  prepareSessions(ctx);
  prepareVerifier(ctx);
  prepareDTLSCookies(ctx);

  std::error_code result = loadCertificateChain(ctx, certData);
  if (result) {
    log_debug("Identity: loadCertificateChain failed", result);
    return result;
  }

  result = loadPrivateKey(ctx, privKey);
  if (result) {
    log_debug("Identity: loadPrivateKey failed", result);
    return result;
  }

  result = loadVerifier(ctx, verifyData);
  if (result) {
    log_debug("Identity: loadVerifier failed", result);
  }

  return result;
}

void Identity::prepareOptions(SSL_CTX *ctx) {
  (void)SSL_CTX_clear_options(ctx, SSL_OP_LEGACY_SERVER_CONNECT);
  auto options = SSL_CTX_set_options(
      ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TICKET);

  if (options)
    log_debug("Identity::prepareContextOptions: options", log::hex(options));
}

void Identity::prepareSessions(SSL_CTX *ctx) {
#if IDENTITY_SESSIONS_ENABLED
  uint8_t id[] = "ofpx";
  SSL_CTX_set_session_id_context(ctx, id, sizeof(id) - 1);
  SSL_CTX_set_timeout(ctx, 60 * 5);
  SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_SERVER);
#endif  // IDENTITY_SESSIONS_ENABLED
}

inline std::error_code sslError(uint32_t err) {
  return asio::error_code(static_cast<int>(err),
                          asio::error::get_ssl_category());
}

std::error_code Identity::loadCertificateChain(SSL_CTX *ctx,
                                               const std::string &certData) {
  // This code is adapted from SSL_CTX_use_certificate_chain_file implementation
  // in ssl_rsa.c

  ERR_clear_error();  // clear error stack for SSL_CTX_use_certificate()

  // The first certificate in the file is special:
  //
  // "X509_AUX is the name given to a certificate with extra info tagged on
  // the end. Since these functions set how a certificate is trusted they should
  // only be used when the certificate comes from a reliable source such as
  // local storage." [Source: d2i_X509_AUX comment in x_x509.c]

  MemBio bio{certData};
  MemX509 mainCert{PEM_read_bio_X509_AUX(bio.get(), nullptr, nullptr, nullptr)};

  if (!mainCert) {
    log_debug("loadCertificateChain failed reading main cert");
    return sslError(ERR_R_PEM_LIB);
  }

  int rc = SSL_CTX_use_certificate(ctx, mainCert.get());

  if (rc == 0 || ERR_peek_error() != 0) {
    log_debug("loadCertificateChain failed: SSL_CTX_use_certificate");
    return sslError(::ERR_get_error());
  }

  SSL_CTX_clear_chain_certs(ctx);

  while (true) {
    MemX509 caCert{PEM_read_bio_X509(bio.get(), nullptr, nullptr, nullptr)};
    if (!caCert)
      break;

    if (!SSL_CTX_add0_chain_cert(ctx, caCert.get())) {
      log_debug("loadCertificateChain failed: SSL_CTX_add0_chain_cert");
      return sslError(::ERR_get_error());
    }

    // Do not free caCert if it was successfully added to the chain.
    caCert.release();
  }

  // When the while loop ends, it's usually just EOF.
  uint32_t err = ERR_peek_last_error();
  assert(err);

  if (ERR_GET_LIB(err) == ERR_LIB_PEM &&
      ERR_GET_REASON(err) == PEM_R_NO_START_LINE) {
    ERR_clear_error();
    return {};
  }

  // Return the error.
  return sslError(::ERR_get_error());
}

/// Load private key from a PEM file.
std::error_code Identity::loadPrivateKey(SSL_CTX *ctx,
                                         const std::string &keyData) {
  ::ERR_clear_error();

  MemBio bio{keyData};

  std::unique_ptr<EVP_PKEY, void (*)(EVP_PKEY *)> privateKey{
      ::PEM_read_bio_PrivateKey(bio.get(), nullptr, nullptr, nullptr),
      EVP_PKEY_free};

  if (!privateKey) {
    log_debug("loadPrivateKey: PEM_read_bio_PrivateKey failed");
    return sslError(::ERR_get_error());
  }

  if (::SSL_CTX_use_PrivateKey(ctx, privateKey.get()) != 1) {
    return sslError(::ERR_get_error());
  }

  if (::SSL_CTX_check_private_key(ctx) != 1) {
    log_warning("loadPrivateKey: private key does not match certificate");
  }

  return {};
}

std::error_code Identity::loadVerifier(SSL_CTX *ctx,
                                       const std::string &verifyData) {
  ::ERR_clear_error();

  MemBio bio{verifyData};
  X509_STORE *store = ::SSL_CTX_get_cert_store(ctx);
  log::fatal_if_null(store);
  int count = 0;

  while (true) {
    MemX509 caCert{PEM_read_bio_X509(bio.get(), nullptr, nullptr, nullptr)};
    if (!caCert) {
      break;
    }

    if (::X509_STORE_add_cert(store, caCert.get()) != 1) {
      log_debug("loadVerifier failed: X509_STORE_add_cert");
      return sslError(::ERR_get_error());
    }

    // Add CA certificate name to our client CA list.
    if (::SSL_CTX_add_client_CA(ctx, caCert.get()) != 1) {
      log_debug("loadVerifier failed: SSL_CTX_add_client_CA");
      return sslError(::ERR_get_error());
    }

    ++count;
  }

  if (count > 0) {
    // When the while loop ends, it's usually just EOF. We expect to read at
    // least one cert.
    uint32_t err = ERR_peek_last_error();
    assert(err);
    if (ERR_GET_LIB(err) == ERR_LIB_PEM &&
        ERR_GET_REASON(err) == PEM_R_NO_START_LINE) {
      ERR_clear_error();
      return {};
    }
  }

  // Return the error.
  return sslError(::ERR_get_error());
}

void Identity::prepareVerifier(SSL_CTX *ctx) {
  // By default, set up the context to REJECT all certificates. The TLS
  // connection object should have its verify_callback set explicitly.

  auto verifyCallback = [](int preverify_ok, X509_STORE_CTX *storeCtx) -> int {
    log_warning(
        "Context TLS verifier is rejecting all certificates - you need to "
        "override!");
    return 0;
  };

  ::SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
                       verifyCallback);
}

void Identity::prepareDTLSCookies(SSL_CTX *ctx) {
#if !defined(OPENSSL_IS_BORINGSSL)
  SSL_CTX_set_cookie_generate_cb(ctx, dtls_cookie_generate_callback);
  SSL_CTX_set_cookie_verify_cb(ctx, dtls_cookie_verify_callback);
#endif  // !defined(OPENSSL_IS_BORINGSSL)
}

int Identity::dtls_cookie_generate_callback(SSL *ssl, uint8_t *cookie,
                                            size_t *cookie_len) {
  log_debug("openssl_cookie_generate_callback",
            std::make_pair("connid", GetConnectionPtr(ssl)->connectionId()));
  memset(cookie, 0, 16);
  for (UInt8 i = 0; i < 16; ++i) {
    cookie[i] = i;
  }
  *cookie_len = 16;
  return 1;
}

int Identity::dtls_cookie_verify_callback(SSL *ssl, const uint8_t *cookie,
                                          size_t cookie_len) {
  log_debug("openssl_cookie_verify_callback",
            std::make_pair("connid", GetConnectionPtr(ssl)->connectionId()));
  return 1;
}
