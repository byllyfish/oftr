// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_ERROR_H_
#define OFP_ERROR_H_

#include "ofp/protocolmsg.h"
#include "ofp/padding.h"
#include "ofp/bytelist.h"

namespace ofp {

/// \brief Implements Error protocol message.
class Error : public ProtocolMsg<Error, OFPT_ERROR, 12, 65535, false> {
 public:
  UInt16 errorType() const { return type_; }
  UInt16 errorCode() const { return code_; }
  ByteRange errorData() const;

  bool validateInput(Validation *context) const { return true; }

 private:
  Header header_;
  Big16 type_ = 0;
  Big16 code_ = 0;

  // Only ErrorBuilder can construct an actual instance.
  Error() : header_{type()} {}

  friend class ErrorBuilder;
};

static_assert(sizeof(Error) == 12, "Unexpected size.");
static_assert(IsStandardLayout<Error>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<Error>(), "Expected trivially copyable.");

/// \brief Implements Error protocol message builder.
class ErrorBuilder {
 public:
  explicit ErrorBuilder(UInt32 xid);
  explicit ErrorBuilder(const Error *msg);

  void setErrorType(UInt16 type) { msg_.type_ = type; }
  void setErrorCode(UInt16 code) { msg_.code_ = code; }

  void setErrorData(const void *data, size_t length) {
    data_.set(data, length);
  }

  void setErrorData(const Message *message);

  void send(Writable *channel);

 private:
  Error msg_;

  Padding<4> padNotPartOfPkt_;
  ByteList data_;
};

}  // namespace ofp

#endif  // OFP_ERROR_H_
