// Copyright 2014-present Bill Fisher. All rights reserved.

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

using HelloIterator =
    ProtocolIterator<HelloElement, ProtocolIteratorType::HelloElement>;
using HelloRange = ProtocolRange<HelloIterator>;

}  // namespace detail

/// \brief Immutable OpenFlow `Hello` protocol message.
class Hello : public ProtocolMsg<Hello, OFPT_HELLO, 8, 65528> {
 public:
  ProtocolVersions protocolVersions() const;

  bool validateInput(Validation *context) const;

 private:
  Header header_;

  detail::HelloRange helloElements() const { return msgBody(); }

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

static_assert(sizeof(HelloBuilder) == 16, "Unexpected size.");
static_assert(IsStandardLayout<HelloBuilder>(), "Expected standard layout.");

}  // namespace ofp

#endif  // OFP_HELLO_H_
