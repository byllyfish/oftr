// Copyright (c) 2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_SYS_SECURITYCHECK_H_
#define OFP_SYS_SECURITYCHECK_H_

#include "ofp/sys/asio_utils.h"

namespace ofp {
namespace sys {

class Connection;

class SecurityCheck {
public:
  // SecurityCheck is a wrapper for related functions.

  template <class NativeSocketType>
  static void beforeHandshake(Connection *conn, NativeSocketType *ssl,
                              bool isClient) {}

  template <class NativeSocketType>
  static void afterHandshake(Connection *conn, NativeSocketType *ssl,
                             std::error_code err) {}

  template <class NativeSocketType>
  static void beforeClose(Connection *conn, NativeSocketType *ssl) {}
};

#if LIBOFP_ENABLE_OPENSSL

template <>
void SecurityCheck::beforeHandshake<SSL>(Connection *conn, SSL *ssl, bool isClient);

template <>
void SecurityCheck::afterHandshake<SSL>(Connection *conn, SSL *ssl, std::error_code err);

template <>
void SecurityCheck::beforeClose<SSL>(Connection *conn, SSL *ssl);

#endif // LIBOFP_ENABLE_OPENSSL

}  // namespace sys
}  // namespace ofp

#endif // OFP_SYS_SECURITYCHECK_H_
