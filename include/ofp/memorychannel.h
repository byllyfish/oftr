//  ===== ---- ofp/memorychannel.h -------------------------*- C++ -*- =====  //
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
/// \brief Defines the MemoryChannel class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_MEMORYCHANNEL_H_
#define OFP_MEMORYCHANNEL_H_

#include "ofp/writable.h"
#include "ofp/bytelist.h"
#include "ofp/padding.h"
#include "ofp/constants.h"

namespace ofp {

class MemoryChannel : public Writable {
public:
  explicit MemoryChannel(UInt8 version = OFP_VERSION_LAST) : version_{version} {}

  const UInt8 *data() const {
    assert(flushed_ || size() == 0);
    return buf_.data();
  }

  size_t size() const { return buf_.size(); }

  UInt8 version() const override { return version_; }

  void setVersion(UInt8 version) { version_ = version; }

  void write(const void *data, size_t length) override {
    buf_.add(data, length);
  }

  void flush() override { flushed_ = true; }

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
