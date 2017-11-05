// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_MEMORYCHANNEL_H_
#define OFP_MEMORYCHANNEL_H_

#include "ofp/bytelist.h"
#include "ofp/constants.h"
#include "ofp/padding.h"
#include "ofp/writable.h"

namespace ofp {

class MemoryChannel : public Writable {
 public:
  explicit MemoryChannel(UInt8 version = OFP_VERSION_LAST)
      : version_{version} {}

  const UInt8 *data() const {
    assert(flushed_ || size() == 0);
    return buf_.data();
  }

  size_t size() const { return buf_.size(); }

  const UInt8 *begin() const { return buf_.begin(); }
  const UInt8 *end() const { return buf_.end(); }

  UInt8 version() const override { return version_; }

  void setVersion(UInt8 version) { version_ = version; }

  void write(const void *data, size_t length) override {
    buf_.add(data, length);
  }

  void flush() override { flushed_ = true; }
  bool mustFlush() const override { return false; }

  UInt32 nextXid() override { return nextXid_++; }

  void setNextXid(UInt32 xid) { nextXid_ = xid; }

  void clear() { buf_.clear(); }

 private:
  ByteList buf_;
  UInt32 nextXid_ = 1;
  UInt8 version_;
  bool flushed_ = false;
  Padding<2> pad_;
};

}  // namespace ofp

#endif  // OFP_MEMORYCHANNEL_H_
