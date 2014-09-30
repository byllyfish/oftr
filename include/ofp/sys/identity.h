//  ===== ---- ofp/identity.h ------------------------------*- C++ -*- =====  //
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
/// \brief Implements Identity class which combines a certificate and private
/// key to provide a TLS context.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_SYS_IDENTITY_H_
#define OFP_SYS_IDENTITY_H_

#include "ofp/sys/asio_utils.h"

namespace ofp {
namespace sys {

class Identity {
 public:
  explicit Identity(const std::string &certFile, std::error_code &error);

  UInt64 securityId() const { return securityId_; }
  void setSecurityId(UInt64 securityId) { securityId_ = securityId; }

  asio::ssl::context *securityContext() { return &context_; }

 private:
  UInt64 securityId_;
  asio::ssl::context context_;
};

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_IDENTITY_H_
