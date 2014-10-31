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
#include "ofp/sys/memx509.h"

using namespace ofp::sys;


// First call to SSL_CTX_get_ex_new_index works around an issue in ASIO where
// it uses SSL_CTX_get_app_data.

const int SSL_CTX_ASIO_PTR = SSL_CTX_get_ex_new_index(0, (void*)"ctx_asio", nullptr, nullptr, nullptr);
const int SSL_CTX_IDENTITY_PTR = SSL_CTX_get_ex_new_index(0, (void*)"ctx_identity", nullptr, nullptr, nullptr);

const int SSL_ASIO_PTR = SSL_get_ex_new_index(0, (void*)"ssl_asio", nullptr, nullptr, nullptr);
const int SSL_IDENTITY_PTR = SSL_get_ex_new_index(0, (void*)"ssl_identity", nullptr, nullptr, nullptr);

constexpr asio::ssl::context_base::method defaultMethod() {
 #if defined(SSL_TXT_TLSV1_2)
  return asio::ssl::context_base::tlsv12;
 #elif defined(SSL_TXT_TLSV1_1)
  return asio::ssl::context_base::tlsv11;
 #else
  return asio::ssl::context_base::tlsv1;
 #endif
}

Identity::Identity(const std::string &certData, const std::string &keyPassphrase, const std::string &verifyData, std::error_code &error)
    : context_{defaultMethod()} {

  error = configureContext();
  if (error) return;

  error = loadCertificateChain(certData);
  if (error) return;

  error = loadPrivateKey(certData, keyPassphrase);
  if (error) return;

  error = loadVerifier(verifyData);
  if (error) return;

  error = prepareVerifier();
  if (error) return;

  // Save subject name of the certificate.
  MemX509 cert{certData};
  subjectName_ = cert.subjectName();
}

Identity::~Identity() {
  for (auto &item : clientSessions_) {
    SSL_SESSION_free(item.second);
  }
}

SSL_SESSION *Identity::findClientSession(const IPv6Endpoint &remoteEndpt) {
  auto iter = clientSessions_.find(remoteEndpt);
  if (iter == clientSessions_.end())
    return nullptr;

  return iter->second;
}

void Identity::saveClientSession(const IPv6Endpoint &remoteEndpt, SSL_SESSION *session) {
  auto ptr = &clientSessions_[remoteEndpt];
  auto prevSession = *ptr;
  *ptr = session;
  if (prevSession) {
    SSL_SESSION_free(prevSession);
  }
}



std::error_code Identity::configureContext() {
  std::error_code err;
  context_.set_options(asio::ssl::context::no_sslv2 | asio::ssl::context::no_sslv3 | asio::ssl::context::no_compression, err);

  uint8_t id[] = "ofpx";
  SSL_CTX_set_session_id_context(context_.native_handle(), id, sizeof(id) - 1);
  SSL_CTX_set_timeout(context_.native_handle(), 60*5);
  SSL_CTX_set_session_cache_mode(context_.native_handle(), SSL_SESS_CACHE_SERVER);

  // Allow for retrieving this Identity object given just an SSL context.
  SSL_CTX_set_ex_data(context_.native_handle(), SSL_CTX_IDENTITY_PTR, this);

  return err;
}


/// Load certificate from a PEM file.
std::error_code Identity::loadCertificateChain(const std::string &certData) {
  std::error_code err;

  asio::const_buffer certBuffer{certData.data(), certData.size()};
  context_.use_certificate_chain(certBuffer, err);

  if (err) {
    log::error("Failed to load certificate", err);
  }

  return err;
}

/// Load private key from a PEM file.
std::error_code Identity::loadPrivateKey(const std::string &keyData, const std::string &passphrase) {
  std::error_code err;

  // Set the context's password callback to return the given passphrase.
  context_.set_password_callback(
    [&passphrase](size_t maxLength, asio::ssl::context::password_purpose purpose) -> std::string {
    return (purpose == asio::ssl::context::for_reading) ? passphrase : "";
  }, err);

  if (err) {
    log::warning("loadPrivateKey: Error returned from set_password_callback", err);
  }

  asio::const_buffer keyBuffer{keyData.data(), keyData.size()};
  context_.use_private_key(keyBuffer, asio::ssl::context::pem, err);

  if (err) {
    log::error("Failed to load private key", err);
  }

  // After loading the key, reset the context's password callback.
  std::error_code ignore;
  context_.set_password_callback(
    [](size_t maxLength, asio::ssl::context::password_purpose purpose) -> std::string {
      return "";
    }, ignore);

  if (!err) {
    // Make sure the key matches the certificate.
    int valid = SSL_CTX_check_private_key(context_.native_handle());
    if (!valid) {
      log::warning("loadPrivateKey: private key does not match certificate");
    }
  }

  return err;
}


std::error_code Identity::loadVerifier(const std::string &verifyData) {
  std::error_code err;

  asio::const_buffer verifyBuffer{verifyData.data(), verifyData.size()};
  context_.add_certificate_authority(verifyBuffer, err);

  if (err) {
    log::error("Failed to load verifier:", err);
  } else {
    // Add CA certificate name to our client CA list.
    err = addClientCA(verifyData);
  }

  return err;
}


std::error_code Identity::addClientCA(const std::string &verifyData) {
  ::ERR_clear_error();

  MemX509 cert{verifyData};
  if (cert) {
    if (::SSL_CTX_add_client_CA(context_.native_handle(), cert) == 1) {
      return std::error_code{};
    }
  }

  return std::error_code(static_cast<int>(::ERR_get_error()), asio::error::get_ssl_category());
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

bool Identity::verifyPeer(UInt64 connId, UInt64 securityId, bool preverified, asio::ssl::verify_context &ctx) {
  int error = X509_STORE_CTX_get_error(ctx.native_handle());
  int depth = X509_STORE_CTX_get_error_depth(ctx.native_handle());

  MemX509 cert{X509_STORE_CTX_get_current_cert(ctx.native_handle())};
  log::fatal_if_null(cert.ptr());

  std::string subjectName = cert.subjectName();

  if (!preverified || (error != X509_V_OK)) {
    // We failed pre-verification or there is a verify error.
    log::warning("Certificate verify failed:", X509_verify_cert_error_string(error), std::make_pair("connid", connId));
    log::warning("Peer certificate", depth, subjectName, std::make_pair("tlsid", securityId), std::make_pair("connid", connId), '\n', cert.toString());
    return false;
  }

  if (depth > 0) {
    log::debug("Verify peer chain:", depth, subjectName, std::make_pair("tlsid", securityId), std::make_pair("connid", connId));
    return preverified;
  }

  assert(depth == 0);
  log::info("Verify peer:", subjectName, std::make_pair("tlsid", securityId), std::make_pair("connid", connId));

  return true;
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


template <>
void Identity::beforeHandshake<EncryptedSocket>(UInt64 connId, EncryptedSocket &sock, const IPv6Endpoint &remoteEndpt, bool isClient) {
  // Store identity pointer in SSL object app data field.
  SSL_CTX *ctxt = SSL_get_SSL_CTX(sock.native_handle());
  Identity *identity = reinterpret_cast<Identity *>(SSL_CTX_get_ex_data(ctxt, SSL_CTX_IDENTITY_PTR));
  SSL_set_ex_data(sock.native_handle(), SSL_IDENTITY_PTR, identity);
  log::fatal_if_null(identity);

  std::error_code err;
  UInt64 securityId = identity->securityId();
  sock.set_verify_callback([connId, securityId](bool preverified, asio::ssl::verify_context &ctx) -> bool {
    return verifyPeer(connId, securityId, preverified, ctx);
  }, err);

  if (err) {
    log::error("Failed to specify TLS verifier callback", err);
  }

  if (isClient) {
    // Check if there is a client session we can resume. This is a static method
    // so we have to retrieve the Identity object from the SSL_CTX.
    
    SSL_SESSION *session = identity->findClientSession(remoteEndpt);
    if (session) {
      SSL_set_session(sock.native_handle(), session);
    }
  }
}


template <>
void Identity::afterHandshake<EncryptedSocket>(UInt64 connId, EncryptedSocket &sock, const IPv6Endpoint &remoteEndpt, bool isClient, std::error_code err) 
{ 
  Identity *identity = reinterpret_cast<Identity *>(SSL_get_ex_data(sock.native_handle(), SSL_IDENTITY_PTR));
  log::fatal_if_null(identity);
  UInt64 securityId = identity->securityId();

  if (err) {
    log::error("TLS handshake failed", std::make_pair("tlsid", securityId), std::make_pair("connid", connId), err);
    return;
  }

  assert(SSL_get_verify_result(sock.native_handle()) == X509_V_OK);

  const char *tlsVersion = SSL_get_version(sock.native_handle());
  const char *tlsCipherSpec = SSL_get_cipher_name(sock.native_handle());
  const bool sessionResumed = SSL_session_reused(sock.native_handle());
  const char *sessionStatus = sessionResumed ? "resumed" : "started";

  std::string sessionId;
  if (sessionResumed) {
    unsigned int idlen;
    const UInt8 *id = SSL_SESSION_get_id(SSL_get_session(sock.native_handle()), &idlen);
    sessionId = RawDataToHex(id, idlen);
  }

  log::info(tlsVersion, "session", sessionStatus, tlsCipherSpec, sessionId, std::make_pair("tlsid", securityId), std::make_pair("connid", connId));

  // If this is a client handshake, save the client session.
  if (isClient) {
    identity->saveClientSession(remoteEndpt, SSL_get1_session(sock.native_handle()));
  }
}

