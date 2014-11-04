// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_MEMX509_H_
#define OFP_MEMX509_H_

#include <openssl/x509.h>

namespace ofp {
namespace sys {

class MemX509 {
 public:
  explicit MemX509(X509 *cert, bool own = false) : cert_{cert}, own_{own} {}

  explicit MemX509(const std::string &data) : own_{true} {
    MemBio bio{data};
    if (bio) {
      cert_ = PEM_read_bio_X509(bio, 0, 0, 0);
    }
  }

  ~MemX509() {
    if (own_ && cert_) ::X509_free(cert_);
  }

  X509 *ptr() const { return cert_; }

  operator X509 *() const noexcept { return cert_; }

  std::string subjectName() {
    assert(cert_);
    X509_NAME *name = X509_get_subject_name(cert_);
    MemBio bio;
    if (name) {
      X509_NAME_print_ex(bio, name, 0, 0);
    }
    return bio.toString();
  }

  std::string toString() {
    assert(cert_);
    MemBio bio;
    X509_print_ex(bio, cert_, 0, 0);
    return bio.toString();
  }

 private:
  X509 *cert_ = nullptr;
  bool own_;
};

}  // namespace sys
}  // namespace ofp

#endif  // OFP_MEMX509_H_
