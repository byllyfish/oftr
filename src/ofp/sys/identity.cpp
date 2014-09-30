//  ===== ---- ofp/identity.cpp ----------------------------*- C++ -*- =====  //
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

#include "ofp/sys/identity.h"

using namespace ofp::sys;

Identity::Identity(const std::string &certFile, std::error_code &error)
    : context_{asio::ssl::context::tlsv1} {
  context_.set_options(
      asio::ssl::context::no_sslv2 | asio::ssl::context::no_sslv3, error);

  if (!error) {
    context_.use_certificate_chain_file(certFile, error);
  }

  if (!error) {
    context_.use_private_key_file(certFile, asio::ssl::context::pem, error);
  }

  if (error) {
    log::error("Failed to construct identity:", certFile, error);
  }
}
