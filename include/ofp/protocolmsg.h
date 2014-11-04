// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_PROTOCOLMSG_H_
#define OFP_PROTOCOLMSG_H_

#include "ofp/header.h"
#include "ofp/message.h"

namespace ofp {

class Validation;

/// \brief Static base class template for all protocol message types. This class
/// implements the two static methods, type() and cast(), common to all protocol
/// message types. This class also specifies the minimum and maximum length of
/// a message, and whether the length of the message must be a multiple of 8.

template <class MsgClass, OFPType MsgType, size_t MsgMinLength = 8,
          size_t MsgMaxLength = 65528, bool MsgMultiple8 = true>
class ProtocolMsg {
 public:
  static_assert(MsgMinLength >= 8, "MinLength must be >= 8");
  static_assert(MsgMaxLength <= 65535, "MaxLength must be <= 2^16-1");

  enum : size_t {
    MinLength = MsgMinLength,
    MaxLength = MsgMaxLength,
    Multiple8 = MsgMultiple8 && (MsgMinLength != MsgMaxLength)
  };

  /// \returns OpenFlow message type represented by this class.
  static constexpr OFPType type() { return MsgType; }

  /// \returns Pointer to message memory as given class or nullptr.
  static const MsgClass *cast(const Message *message) {
    // FIXME - why is this in Message? Why not inline here?
    return message->castMessage<MsgClass>();
  }

  /// \returns true if message length is potentially valid.
  static bool isLengthValid(size_t length) {
    return (length >= MinLength) && (length <= MaxLength) &&
           (!Multiple8 || ((length % 8) == 0));
  }

  const Header *msgHeader() const {
    return reinterpret_cast<const Header *>(this);
  }

  ByteRange msgBody() const {
    return ByteRange{msgHeader() + 1, msgHeader()->length() - sizeof(Header)};
  }
};

}  // namespace ofp

#endif  // OFP_PROTOCOLMSG_H_
