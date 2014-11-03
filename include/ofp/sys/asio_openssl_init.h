//  ===== ---- ofp/sys/asio_openssl_init.h------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Specialize asio's openssl_init<> template so it does *not* initialize
/// openssl statically before main().
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_SYS_ASIO_OPENSSL_INIT_H_
#define OFP_SYS_ASIO_OPENSSL_INIT_H_

#include <asio/ssl/detail/openssl_init.hpp>

namespace asio {
namespace ssl {
namespace detail {

template <>
class openssl_init<> : private openssl_init_base {
 public:
  // Constructor.
  openssl_init() : ref_(instance()) {}

  // Destructor.
  ~openssl_init() {}

#if !defined(SSL_OP_NO_COMPRESSION) && (OPENSSL_VERSION_NUMBER >= 0x00908000L)
  using openssl_init_base::get_null_compression_methods;
#endif  // !defined(SSL_OP_NO_COMPRESSION)
        // && (OPENSSL_VERSION_NUMBER >= 0x00908000L)

  using openssl_init_base::instance;

 private:
  asio::detail::shared_ptr<do_init> ref_;
};

}  // namespace detail
}  // namespace ssl
}  // namespace asio

#endif  // OFP_SYS_ASIO_OPENSSL_INIT_H_
