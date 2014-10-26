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
#include "ofp/sys/membio.h"
#include <openssl/x509.h>

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

  // By default, set up the context to REJECT all certificates. The TLS connection
  // object should have its verify_callback set instead.

  context_.set_verify_callback([this](bool preverified, asio::ssl::verify_context &ctx) -> bool {
    log::warning("Context TLS verifier is rejecting all certificates - you need to override!");
    return false;
  }, err);

  return err;
}

static std::string getSubjectName(X509 *cert);

bool Identity::verifyPeer(UInt64 connId, bool preverified, asio::ssl::verify_context &ctx) {
  int depth = X509_STORE_CTX_get_error_depth(ctx.native_handle());
  X509 *cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());

  if (!preverified) {
    // We failed pre-verification.
    MemBio certBio;
    X509_print_ex(certBio, cert, 0, 0);
    log::warning("TLS verify peer failed:", getSubjectName(cert), std::make_pair("connid", connId), '\n', certBio.toString());
    return false;
  }

  if (depth > 0) {
    log::debug("TLS verify peer chain:", depth, getSubjectName(cert), std::make_pair("connid", connId));
    return preverified;
  }

  assert(depth == 0);

  log::info("TLS verify peer:", getSubjectName(cert), std::make_pair("connid", connId));

  return true;
}


static std::string getSubjectName(X509 *cert) {
  X509_NAME *name = X509_get_subject_name(cert);
  ofp::log::fatal_if_null(name);
  MemBio bio;
  X509_NAME_print_ex(bio, name, 0, 0);
  return bio.toString();
}

#if 0
static std::string getSubjectAltName(X509 *cert) {
  std::string result;

  GENERAL_NAMES* gens = static_cast<GENERAL_NAMES*>(X509_get_ext_d2i(cert, NID_subject_alt_name, 0, 0));
  for (int i = 0; i < sk_GENERAL_NAME_num(gens); ++i) {
    GENERAL_NAME* gen = sk_GENERAL_NAME_value(gens, i);
    if (gen->type == GEN_DNS) {
      ASN1_IA5STRING* domain = gen->d.dNSName;
      if (domain->type == V_ASN1_IA5STRING && domain->data && domain->length) {
        result += "DNS:" + std::string((char *)domain->data, domain->length) + '\n';
      } else {
        result += "DNS:?\n";
      }
    } else if (gen->type == GEN_IPADD) {
      ASN1_OCTET_STRING* ip_address = gen->d.iPAddress;
      if (ip_address->type == V_ASN1_OCTET_STRING && ip_address->data) {
        if (ip_address->length == 4) {
          result += "IP:v4\n";
        } else if (ip_address->length == 16) {
          result += "IP:v6\n";
        } else {
          result += "IP:?";
        }
      } else {
        result += "IP:?\n";
      }
    } else if (gen->type == GEN_EMAIL) {
      ASN1_OCTET_STRING *email = gen->d.rfc822Name;
      if (email->type == V_ASN1_IA5STRING && email->data && email->length) {
        result += "email:" + std::string((char *)email->data, email->length) + '\n';
      } else {
        result += "email:?\n";
      }
    }
  }
  GENERAL_NAMES_free(gens);

  return result;
}
#endif //0

