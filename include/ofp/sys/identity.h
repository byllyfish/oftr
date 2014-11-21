// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_SYS_IDENTITY_H_
#define OFP_SYS_IDENTITY_H_

#include "ofp/sys/asio_utils.h"
#include "ofp/sys/buffered.h"
#include <unordered_map>

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

  asio::ssl::context *securityContext() { return &context_; }
  std::string subjectName() const { return subjectName_; }

  SSL_SESSION *findClientSession(const IPv6Endpoint &remoteEndpt);
  void saveClientSession(const IPv6Endpoint &remoteEndpt, SSL_SESSION *session);

  template <class SocketType>
  static void beforeHandshake(Connection *conn, SocketType *ssl,
                              bool isClient) {}

  template <class SocketType>
  static void afterHandshake(Connection *conn, SocketType *ssl,
                             std::error_code err) {}

 private:
  UInt64 securityId_;
  asio::ssl::context context_;
  std::string subjectName_;
  std::unordered_map<IPv6Endpoint, SSL_SESSION *> clientSessions_;

  std::error_code configureContext();
  std::error_code loadCertificateChain(const std::string &certData);
  std::error_code loadPrivateKey(const std::string &keyData,
                                 const std::string &keyPassphrase);
  std::error_code loadVerifier(const std::string &verifyData);
  std::error_code addClientCA(const std::string &verifyData);
  std::error_code prepareVerifier();
  std::error_code prepareCookies();

  // static bool verifyPeer(UInt64 connId, UInt64 securityId, bool preverified,
  //                      X509_STORE_CTX *ctx);

  static int openssl_verify_callback(int preverified, X509_STORE_CTX *ctx);
  static int openssl_cookie_generate_callback(SSL *ssl, unsigned char *cookie,
                                              unsigned int *cookie_len);
  static int openssl_cookie_verify_callback(SSL *ssl, unsigned char *cookie,
                                            unsigned int cookie_len);
};

template <>
void Identity::beforeHandshake<SSL>(Connection *conn, SSL *ssl, bool isClient);

template <>
void Identity::afterHandshake<SSL>(Connection *conn, SSL *ssl,
                                   std::error_code err);

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_IDENTITY_H_
