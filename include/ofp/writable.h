// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_WRITABLE_H_
#define OFP_WRITABLE_H_

#include "ofp/padding.h"
#include "ofp/types.h"

namespace ofp {

class Writable {
 public:
  virtual ~Writable() {}

  virtual UInt8 version() const = 0;
  virtual UInt32 nextXid() = 0;

  virtual void write(const void *data, size_t length) = 0;
  virtual void flush() = 0;

  void write(const void *data, size_t length, size_t padSize) {
    assert(padSize < 8);
    Padding<8> pad;
    write(data, length);
    write(&pad, padSize);
  }

  virtual bool mustFlush() const { return true; }
};

}  // namespace ofp

#endif  // OFP_WRITABLE_H_
