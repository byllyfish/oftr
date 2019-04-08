// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/sys/identity.h"
#include "llvm/Support/FileSystem.h"
#include "ofp/sys/connection.h"
#include "ofp/sys/membio.h"
#include "ofp/sys/memx509.h"

using namespace ofp;
using namespace ofp::sys;

// First call to SSL_CTX_get_ex_new_index works around an issue in ASIO where
// it uses SSL_CTX_get_app_data.

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

static const int SSL_CTX_ASIO_PTR =
    SSL_CTX_get_ex_new_index(0, nullptr, nullptr, nullptr, nullptr);
static const int SSL_CTX_IDENTITY_PTR =
    SSL_CTX_get_ex_new_index(0, nullptr, nullptr, nullptr, nullptr);

static const int SSL_ASIO_PTR =
    SSL_get_ex_new_index(0, nullptr, nullptr, nullptr, nullptr);
static const int SSL_CONNECTION_PTR =
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
                   const std::string &verifyData, const std::string &version,
                   const std::string &ciphers, const std::string &keyLogFile,
                   std::error_code &error)
    : tls_{asio::ssl::context_base::tlsv12} {
  // Allow for retrieving this Identity object given just an SSL context.
  SetIdentityPtr(tls_.native_handle(), this);

  // Initialize the TLS context.
  error = initContext(tls_.native_handle(), certData, privKey, verifyData,
                      version, ciphers, keyLogFile);
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
                                      const std::string &verifyData,
                                      const std::string &version,
                                      const std::string &ciphers,
                                      const std::string &keyLogFile) {
  std::error_code result = prepareOptions(ctx, ciphers);
  if (result) {
    log_error("Identity: prepareOptions failed", result);
    return result;
  }

  result = prepareVersion(ctx, version);
  if (result) {
    log_error("Identity: prepareVersion failed", result);
    return result;
  }

  prepareSessions(ctx);
  prepareVerifier(ctx);

  result = prepareKeyLogFile(ctx, keyLogFile);
  if (result) {
    log_error("Identity: prepareKeyLogFile failed", result);
    return result;
  }

  result = loadCertificateChain(ctx, certData);
  if (result) {
    log_error("Identity: loadCertificateChain failed", result);
    return result;
  }

  result = loadPrivateKey(ctx, privKey);
  if (result) {
    log_error("Identity: loadPrivateKey failed", result);
    return result;
  }

  result = loadVerifier(ctx, verifyData);
  if (result) {
    log_error("Identity: loadVerifier failed", result);
  }

  return result;
}

std::error_code Identity::prepareOptions(SSL_CTX *ctx,
                                         const std::string &ciphers) {
  (void)SSL_CTX_clear_options(ctx, SSL_OP_LEGACY_SERVER_CONNECT);
  auto options = SSL_CTX_set_options(
      ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TICKET);

  if (options) {
    log_debug("Identity::prepareContextOptions: options", options);
  }

  if (!ciphers.empty()) {
    if (!SSL_CTX_set_cipher_list(ctx, ciphers.c_str())) {
      log_error("SSL_CTX_set_cipher_list failed:", ciphers);
      return std::make_error_code(std::errc::invalid_argument);
    }
  }

  return {};
}

OFP_BEGIN_IGNORE_PADDING

struct VersionInfo {
  const char *name;
  UInt16 version;
};

OFP_END_IGNORE_PADDING

static const VersionInfo sVersionInfo[] = {{"TLS1.0", TLS1_VERSION},
                                           {"TLS1.1", TLS1_1_VERSION},
                                           {"TLS1.2", TLS1_2_VERSION}
#if defined(TLS1_3_VERSION)
                                           ,
                                           {"TLS1.3", TLS1_3_VERSION}
#endif  // defined(TLS1_3_VERSION)
};

static UInt16 sParseVersionCode(llvm::StringRef vers) {
  for (size_t i = 0; i < ArrayLength(sVersionInfo); ++i) {
    if (vers.equals_lower(sVersionInfo[i].name)) {
      return sVersionInfo[i].version;
    }
  }

  return 0;
}

std::error_code Identity::prepareVersion(SSL_CTX *ctx,
                                         const std::string &version) {
  // `version` can be a single version: "TLS1.2" or a range "TLS1.0-TLS1.2".
  // If `version` is empty, ignore it.
  if (version.empty()) {
    return {};
  }

  llvm::StringRef s{version};
  auto pair = s.split('-');

  UInt16 vers_min = sParseVersionCode(pair.first);
  if (!vers_min) {
    log_warning("Identity::prepareVersion: invalid version", pair.first);
    return std::make_error_code(std::errc::invalid_argument);
  }

  UInt16 vers_max = vers_min;
  if (!pair.second.empty()) {
    // Version range.
    vers_max = sParseVersionCode(pair.second);
    if (!vers_max) {
      log_warning("Identity::prepareVersion: invalid version", pair.second);
      return std::make_error_code(std::errc::invalid_argument);
    }
  }

  assert(vers_min > 0);
  assert(vers_max > 0);

  if (!SSL_CTX_set_min_proto_version(ctx, vers_min)) {
    log_warning("Identity::prepareVersion: SSL_CTX_set_min_version failed:",
                vers_min);
    return std::make_error_code(std::errc::invalid_argument);
  }

  if (!SSL_CTX_set_max_proto_version(ctx, vers_max)) {
    log_warning("Identity::prepareVersion: SSL_CTX_set_max_version failed:",
                vers_max);
    return std::make_error_code(std::errc::invalid_argument);
  }

  return {};
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
    log_error("Identity: private key does not match certificate");
    // TODO(bfish): Return error here.
  }

  return {};
}

std::error_code Identity::loadVerifier(SSL_CTX *ctx,
                                       const std::string &verifyData) {
  if (verifyData.empty()) {
    // Don't verify peer certificate. There's no cacert.
    peerVerifyMode_ = SSL_VERIFY_NONE;
    return {};
  }

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

std::error_code Identity::prepareKeyLogFile(SSL_CTX *ctx,
                                            const std::string &keyLogFile) {
  // Set up a callback to log key material for debugging use.
  //
  // The format is described in:
  //   https://developer.mozilla.org/en-US/docs/Mozilla/Projects/NSS/Key_Log_Format

  using namespace llvm::sys;
  
  if (keyLogFile.empty()) {
    // Do nothing if no keylog file specified.
    return {};
  }

  std::error_code err;
  keyLogFile_.reset(
      new llvm::raw_fd_ostream{keyLogFile, err, fs::F_Append | fs::F_Text});
  if (err) {
    log_error("Identity: Failed to open file:", keyLogFile);
    return err;
  }

#if HAVE_SSL_CTX_SET_KEYLOG_CALLBACK
  ::SSL_CTX_set_keylog_callback(ctx, keylog_callback);
  return {};
#else
  return std::make_error_code(std::errc::operation_not_supported);
#endif
}

void Identity::keylog_callback(const SSL *ssl, const char *line) {
  Identity *identity = Identity::GetIdentityPtr(ssl);
  identity->logKeyMaterial(line);
}

void Identity::logKeyMaterial(const char *line) {
  if (keyLogFile_) {
    *keyLogFile_ << line;
    keyLogFile_->flush();
  }
}
