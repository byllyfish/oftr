//  ===== ---- ofp/message.h -------------------------------*- C++ -*- =====  //
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
/// \brief Defines the Message class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_MESSAGE_H
#define OFP_MESSAGE_H

#include "ofp/bytelist.h"
#include "ofp/header.h"
#include "ofp/datapathid.h"

namespace ofp { // <namespace ofp>

class Writable;
class Channel;

namespace sys { // <namespace sys>
class Connection;
} // </namespace sys>

/// \brief Implements a protocol message buffer.
class Message {
public:
  explicit Message(sys::Connection *channel) : channel_{channel} {
    buf_.resize(sizeof(Header));
  }

  Message(const void *data, size_t size) : channel_{nullptr} {
    buf_.add(data, size);
    assert(header()->length() == size);
  }

  UInt8 *mutableData(size_t size) {
    buf_.resize(size);
    return buf_.mutableData();
  }

  void shrink(size_t size) {
    assert(size <= buf_.size());
    buf_.resize(size);
  }

  const Header *header() const {
    return reinterpret_cast<const Header *>(buf_.data());
  }
  Header *mutableHeader() {
    return reinterpret_cast<Header *>(buf_.mutableData());
  }

  const UInt8 *data() const { return buf_.data(); }
  size_t size() const { return buf_.size(); }

  OFPType type() const { return header()->type(); }
  Channel *source() const;
  UInt32 xid() const { return header()->xid(); }
  UInt8 version() const { return header()->version(); }
  bool isRequestType() const;

  // Provides convenient implementation of message cast.
  template <class MsgType>
  const MsgType *cast() const {
    assert(type() == MsgType::type());

    size_t length = size();
    assert(length == header()->length());

    if (length < MsgType::MinLength || length > MsgType::MaxLength || (MsgType::Multiple8 && (length % 8) != 0)) {
      log::info("Invalid length for ", type());
      return nullptr;
    }

    const MsgType *msg = reinterpret_cast<const MsgType *>(data());
    if (!msg->validateLength(length)) {
      return nullptr;
    }

    return msg;
  }

  bool isValidHeader();
  void transmogrify();

private:
  ByteList buf_;
  sys::Connection *channel_;

  friend std::ostream &operator<<(std::ostream &os, const Message &msg);
  friend class Transmogrify;
};

std::ostream &operator<<(std::ostream &os, const Message &msg);

inline std::ostream &operator<<(std::ostream &os, const Message &msg) {
  return os << msg.buf_;
}

} // </namespace ofp>

#endif // OFP_MESSAGE_H
