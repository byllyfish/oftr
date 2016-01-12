// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_SYS_IDENTITY_H_
#define OFP_SYS_IDENTITY_H_

#include <unordered_map>
#include "ofp/sys/asio_utils.h"
#include "ofp/sys/buffered.h"

namespace ofp {
namespace sys {

class Connection;

class Identity {
 public:
  explicit Identity(const std::string &certData,
                    const std::string &keyPassphrase,
                    const std::string &verifyData, std::error_code &error);
  ~Identity();

  UInt64 securityId() const { return securityId_; }
  void setSecurityId(UInt64 securityId) { securityId_ = securityId; }

  asio::ssl::context *tlsContext() { return &tls_; }
  SSL_CTX *dtlsContext() { return dtls_.get(); }

  std::string subjectName() const { return subjectName_; }

  SSL_SESSION *findClientSession(const IPv6Endpoint &remoteEndpt);
  void saveClientSession(const IPv6Endpoint &remoteEndpt, SSL_SESSION *session);

  template <class SocketType>
  static void beforeHandshake(Connection *conn, SocketType *ssl,
                              bool isClient) {}

  template <class SocketType>
  static void afterHandshake(Connection *conn, SocketType *ssl,
                             std::error_code err) {}

  template <class SocketType>
  static void beforeClose(Connection *conn, SocketType *ssl) {}

  static asio::ssl::context *plaintextContext();

 private:
  /// Unique non-zero ID used to reference this Identity.
  UInt64 securityId_ = 0;

  /// SSL context used for TLS connections over TCP.
  asio::ssl::context tls_;

  /// Separate SSL context used for DTLS connections over UDP.
  std::unique_ptr<SSL_CTX, void (*)(SSL_CTX *)> dtls_;

  /// Subject DN of this identities certificate.
  std::string subjectName_;

  /// Map used to store client sessions by IP endpoint. Used for session
  /// resumption in the client.
  std::unordered_map<IPv6Endpoint, SSL_SESSION *> clientSessions_;

  std::error_code initContext(SSL_CTX *ctx, const std::string &certData,
                              const std::string &keyPassphrase,
                              const std::string &verifyData);

  static std::error_code loadCertificateChain(SSL_CTX *ctx,
                                              const std::string &certData);
  static std::error_code loadPrivateKey(SSL_CTX *ctx,
                                        const std::string &keyData,
                                        const std::string &keyPassphrase);
  static std::error_code loadVerifier(SSL_CTX *ctx,
                                      const std::string &verifyData);

  static void prepareOptions(SSL_CTX *ctx);
  static void prepareSessions(SSL_CTX *ctx);
  static void prepareVerifier(SSL_CTX *ctx);
  static void prepareDTLSCookies(SSL_CTX *ctx);

  static int tls_verify_callback(int preverified, X509_STORE_CTX *ctx);
  static int dtls_cookie_generate_callback(SSL *ssl, uint8_t *cookie,
                                           size_t *cookie_len);
  static int dtls_cookie_verify_callback(SSL *ssl, const uint8_t *cookie,
                                         size_t cookie_len);
};

template <>
void Identity::beforeHandshake<SSL>(Connection *conn, SSL *ssl, bool isClient);

template <>
void Identity::afterHandshake<SSL>(Connection *conn, SSL *ssl,
                                   std::error_code err);

template <>
void Identity::beforeClose<SSL>(Connection *conn, SSL *ssl);

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_IDENTITY_H_
