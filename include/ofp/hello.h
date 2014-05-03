//  ===== ---- ofp/hello.h ---------------------------------*- C++ -*- =====  //
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
/// \brief Defines the Hello and HelloBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_HELLO_H_
#define OFP_HELLO_H_

#include "ofp/protocolmsg.h"
#include "ofp/protocolversions.h"
#include "ofp/protocolrange.h"

namespace ofp {
namespace detail {

class HelloElement : private NonCopyable {
 public:
  enum { ProtocolIteratorSizeOffset = 2, ProtocolIteratorAlignment = 8 };

  UInt16 type() const { return type_; }

  void setType(UInt16 type) { type_ = type; }
  void setLength(UInt16 length) { length_ = length; }

  ProtocolVersions versionBitMap() const;

 private:
  Big16 type_;
  Big16 length_;
};

static_assert(sizeof(HelloElement) == 4, "Unexpected size.");
static_assert(IsStandardLayout<HelloElement>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<HelloElement>(),
              "Expected trivially copyable.");

using HelloIterator = ProtocolIterator<HelloElement>;
using HelloRange = ProtocolRange<HelloIterator>;

}  // namespace detail

/// \brief Immutable OpenFlow `Hello` protocol message.
class Hello : public ProtocolMsg<Hello, OFPT_HELLO, 8, 65528> {
 public:
  ProtocolVersions protocolVersions() const;

  bool validateInput(size_t length) const;

 private:
  Header header_;

  detail::HelloRange helloElements() const {
    return msgBody();
  }

  // Only HelloBuilder can construct an actual instance.
  Hello() : header_{type()} {}

  friend class HelloBuilder;
};

static_assert(sizeof(Hello) == 8, "Unexpected size.");
static_assert(IsStandardLayout<Hello>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<Hello>(), "Expected trivially copyable.");

/// \brief Mutable builder for an OpenFlow `Hello` protocol message.
class HelloBuilder {
 public:
  explicit HelloBuilder(ProtocolVersions versions = ProtocolVersions::All)
      : bitmap_{versions.bitmap()} {
    msg_.header_.setVersion(versions.highestVersion());
  }

  explicit HelloBuilder(UInt8 version)
      : bitmap_{ProtocolVersions{version}.bitmap()} {
    msg_.header_.setVersion(version);
  }

  explicit HelloBuilder(const Hello *msg);

  ProtocolVersions protocolVersions() const;
  void setProtocolVersions(ProtocolVersions versions);

  UInt32 send(Writable *channel);

 private:
  Hello msg_;
  detail::HelloElement elem_;
  Big32 bitmap_ = 0;
};

}  // namespace ofp

#endif  // OFP_HELLO_H_
