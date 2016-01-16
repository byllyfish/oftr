// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_SYS_MEMX509_H_
#define OFP_SYS_MEMX509_H_

#include <openssl/x509.h>

namespace ofp {
namespace sys {

OFP_BEGIN_IGNORE_PADDING

class MemX509 {
 public:
  explicit MemX509(X509 *cert, bool own = true) : cert_{cert}, own_{own} {}

  explicit MemX509(const std::string &data)
      : cert_{::PEM_read_bio_X509(MemBio{data}.get(), 0, 0, 0)} {}

  ~MemX509() {
    if (cert_ && own_) {
      ::X509_free(cert_);
      cert_ = nullptr;
    }
  }

  X509 *get() const noexcept { return cert_; }
  void release() noexcept { cert_ = nullptr; }
  bool operator!() const noexcept { return cert_ == nullptr; }

  std::string subjectName() {
    assert(cert_);
    X509_NAME *name = X509_get_subject_name(cert_);
    MemBio bio;
    if (name) {
      X509_NAME_print_ex(bio.get(), name, 0, 0);
    }
    return bio.toString();
  }

  std::string toString() {
    assert(cert_);
    MemBio bio;
    X509_print_ex(bio.get(), cert_, 0, 0);
    return bio.toString();
  }

 private:
  X509 *cert_;
  bool own_ = true;
};

OFP_END_IGNORE_PADDING

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_MEMX509_H_
