// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_SYS_MEMBIO_H_
#define OFP_SYS_MEMBIO_H_

namespace ofp {
namespace sys {

class MemBio {
 public:
  MemBio() : bio_{log::fatal_if_null(BIO_new(BIO_s_mem()))} {}
  explicit MemBio(const std::string &buf)
      : bio_{log::fatal_if_null(BIO_new_mem_buf(
            RemoveConst_cast(buf.data()), static_cast<int>(buf.size())))} {}

  ~MemBio() {
    // Constructor guarantees that bio_ is created, or entire program aborted.
    BIO_free(bio_);
  }

  std::string toString() {
    char *mem;
    auto size = BIO_get_mem_data(bio_, &mem);
    return std::string(mem, Unsigned_cast(size));
  }

  BIO *get() const noexcept { return bio_; }

  bool operator!() const { return bio_ != nullptr; }

 private:
  BIO *bio_;
};

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_MEMBIO_H_
