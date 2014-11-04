// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_WRITABLE_H_
#define OFP_WRITABLE_H_

#include "ofp/types.h"
#include "ofp/padding.h"

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
};

}  // namespace ofp

#endif  // OFP_WRITABLE_H_
