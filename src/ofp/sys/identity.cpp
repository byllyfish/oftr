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

Identity::Identity(const std::string &certFile, const std::string &password, const std::string &verifyFile, std::error_code &error)
    : context_{asio::ssl::context::tlsv1} {

  error = configureContext();
  if (error) return;

  error = loadCertificate(certFile, password);
  if (error) return;

  error = loadVerifier(verifyFile);
  if (error) return;

  error = prepareVerifier();
}


std::error_code Identity::configureContext() {
  std::error_code err;

  context_.set_options(asio::ssl::context::no_sslv2 | asio::ssl::context::no_sslv3 | asio::ssl::context::no_compression, err);

  return err;
}


std::error_code Identity::loadCertificate(const std::string &certFile, const std::string &password) {
  std::error_code err;

  context_.use_certificate_chain_file(certFile, err);

  if (!err) {
    context_.set_password_callback(
      [password](size_t maxLength, asio::ssl::context::password_purpose purpose) -> std::string {
      return password;
    }, err);

    if (!err) {
      context_.use_private_key_file(certFile, asio::ssl::context::pem, err);
    
      if (!err) {
        context_.set_password_callback(
          [password](size_t maxLength, asio::ssl::context::password_purpose purpose) -> std::string {
            return "";
          }, err);
      }
    }
  }

  // TODO - verify that key matches certificate?

  if (err) {
    char *cwd = getcwd(nullptr, 0);
    std::string dir{cwd};
    free(cwd);

    log::error("Failed to construct identity:", certFile, dir, err);
  }

  return err;
}


std::error_code Identity::loadVerifier(const std::string &verifyFile) {
  std::error_code err;

  context_.load_verify_file(verifyFile, err);

  if (err) {
    char *cwd = getcwd(nullptr, 0);
    std::string dir{cwd};
    free(cwd);

    log::error("Failed to load verifier:", verifyFile, dir, err);
  }  

  return err;
}


std::error_code Identity::prepareVerifier() {
  std::error_code err;

  // Set the verify mode to "verify the peer" and "fail verification if peer
  // certificate is not present". In a client handshake, the verify_fail_if
  // no_cert option is ignored -- I'm just setting the verify mode here.
  context_.set_verify_mode(asio::ssl::verify_peer | asio::ssl::verify_fail_if_no_peer_cert, err);

  //context_.set_verify_callback([]())

  return err;
}
