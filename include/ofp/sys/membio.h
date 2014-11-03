#ifndef OFP_SYS_MEMBIO_H_
#define OFP_SYS_MEMBIO_H_

namespace ofp {
namespace sys {

class MemBio {
 public:
  explicit MemBio() : bio_{log::fatal_if_null(BIO_new(BIO_s_mem()))} {}
  explicit MemBio(const std::string &buf)
      : bio_{log::fatal_if_null(
            BIO_new_mem_buf((void *)buf.data(), (int)buf.size()))} {}

  ~MemBio() { BIO_free(bio_); }

  std::string toString() {
    char *mem;
    long size = BIO_get_mem_data(bio_, &mem);
    return std::string(mem, Unsigned_cast(size));
  }

  BIO *ptr() const noexcept { return bio_; }

  operator BIO *() const noexcept { return bio_; }

 private:
  BIO *bio_;
};

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_MEMBIO_H_
