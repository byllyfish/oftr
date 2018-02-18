// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_SYS_IDENTITY_H_
#define OFP_SYS_IDENTITY_H_

#include <unordered_map>
#include "ofp/sys/asio_utils.h"
#include "ofp/sys/buffered.h"

namespace ofp {
namespace sys {

// TLS session support is currently disabled.
#define IDENTITY_SESSIONS_ENABLED 0

class Connection;

OFP_BEGIN_IGNORE_PADDING

class Identity {
 public:
  explicit Identity(const std::string &certData, const std::string &privKey,
                    const std::string &verifyData, const std::string &version,
                    const std::string &ciphers, const std::string &keyLogFile,
                    std::error_code &error);
  ~Identity();

  UInt64 securityId() const { return securityId_; }
  void setSecurityId(UInt64 securityId) { securityId_ = securityId; }

  asio::ssl::context *tlsContext() { return &tls_; }
  SSL_CTX *dtlsContext() { return dtls_.get(); }

  std::string subjectName() const { return subjectName_; }
  int peerVerifyMode() const { return peerVerifyMode_; }

  SSL_SESSION *findClientSession(const IPv6Endpoint &remoteEndpt);
  void saveClientSession(const IPv6Endpoint &remoteEndpt, SSL_SESSION *session);

  static Identity *GetIdentityPtr(SSL_CTX *ctx);
  static Identity *GetIdentityPtr(const SSL *ssl) {
    return GetIdentityPtr(SSL_get_SSL_CTX(ssl));
  }
  static void SetIdentityPtr(SSL_CTX *ctx, Identity *identity);

  static Connection *GetConnectionPtr(SSL *ssl);
  static void SetConnectionPtr(SSL *ssl, Connection *conn);

 private:
  /// Unique non-zero ID used to reference this Identity.
  UInt64 securityId_ = 0;

  /// SSL context used for TLS connections over TCP.
  asio::ssl::context tls_;

  /// Separate SSL context used for DTLS connections over UDP.
  std::unique_ptr<SSL_CTX, void (*)(SSL_CTX *)> dtls_;

  /// Subject DN of this identity's certificate.
  std::string subjectName_;

#if IDENTITY_SESSIONS_ENABLED
  /// Map used to store client sessions by IP endpoint. Used for session
  /// resumption in the client.
  std::unordered_map<IPv6Endpoint, SSL_SESSION *> clientSessions_;
#endif  // IDENTITY_SESSIONS_ENABLED

  /// Peer verification enabled.
  int peerVerifyMode_ = SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT;

  /// Key log file output stream.
  std::unique_ptr<llvm::raw_ostream> keyLogFile_;

  std::error_code initContext(SSL_CTX *ctx, const std::string &certData,
                              const std::string &privKey,
                              const std::string &verifyData,
                              const std::string &version,
                              const std::string &ciphers,
                              const std::string &keyLogFile);

  static std::error_code loadCertificateChain(SSL_CTX *ctx,
                                              const std::string &certData);
  static std::error_code loadPrivateKey(SSL_CTX *ctx,
                                        const std::string &keyData);
  static std::error_code loadVerifier(SSL_CTX *ctx,
                                      const std::string &verifyData);

  static std::error_code prepareOptions(SSL_CTX *ctx,
                                        const std::string &ciphers);
  static std::error_code prepareVersion(SSL_CTX *ctx,
                                        const std::string &version);
  static void prepareSessions(SSL_CTX *ctx);
  static void prepareVerifier(SSL_CTX *ctx);
  static void prepareDTLSCookies(SSL_CTX *ctx);

  std::error_code prepareKeyLogFile(SSL_CTX *ctx,
                                    const std::string &keyLogFile);
  static void keylog_callback(const SSL *ssl, const char *line);
  void logKeyMaterial(const char *line);

  static int dtls_cookie_generate_callback(SSL *ssl, uint8_t *cookie,
                                           size_t *cookie_len);
  static int dtls_cookie_verify_callback(SSL *ssl, const uint8_t *cookie,
                                         size_t cookie_len);
};

OFP_END_IGNORE_PADDING

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_IDENTITY_H_
