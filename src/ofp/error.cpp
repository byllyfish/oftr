// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/error.h"
#include "ofp/log.h"
#include "ofp/message.h"
#include "ofp/writable.h"

using namespace ofp;

ByteRange Error::errorData() const {
  return SafeByteRange(this, header_.length(), sizeof(Error));
}

OFPErrorType Error::errorType() const {
  // The error type values differ in meaning between protocol version 1 and
  // later versions. A type of EXPERIMENTER will always be honored.

  return OFPErrorTypeFromValue(msgHeader()->version(), type_, code_);
}

OFPErrorCode Error::errorCode() const {
  return OFPErrorCodeFromValue(msgHeader()->version(), type_, code_);
}

/// Return a textual description of the error data value.
std::string Error::errorText() const {
  ByteRange data = errorData();

  if (data.isPrintable()) {
    return std::string{reinterpret_cast<const char *>(data.data()),
                       data.size()};
  }

  if (data.size() < 12) {
    return "";
  }

  // Determine the message type.
  if (data[0] <= OFP_VERSION_MAX_ALLOWED) {
    OFPType msgType = Header::translateType(data[0], data[1], OFP_VERSION_4);
    if (msgType != OFPT_UNSUPPORTED) {
      std::string buf;
      llvm::raw_string_ostream ss{buf};
      ss << "Type: " << msgType;
      if (msgType == OFPT_MULTIPART_REQUEST ||
          msgType == OFPT_MULTIPART_REPLY) {
        assert(data.size() >= 12);
        UInt16 type = *Big16_cast(data.data() + 8);
        OFPMultipartType mpType = static_cast<OFPMultipartType>(type);
        ss << "." << mpType;
      }
      return ss.str();
    }
  }

  return "";
}

ErrorBuilder::ErrorBuilder(UInt32 xid) {
  msg_.header_.setXid(xid);
}

ErrorBuilder::ErrorBuilder(const Error *msg) : msg_{*msg} {
  // Only data is left to copy.
  ByteRange data = msg->errorData();
  setErrorData(data.data(), data.size());
}

void ErrorBuilder::setErrorData(const Message *message) {
  assert(message);
  size_t len = message->size();
  if (len > 64)
    len = 64;
  setErrorData(message->data(), len);
}

void ErrorBuilder::setErrorCode(OFPErrorCode code) {
  msg_.type_ = OFPErrorCodeGetType(code);
  msg_.code_ = OFPErrorCodeGetCode(code);
}

void ErrorBuilder::send(Writable *channel) {
  size_t msgLen = sizeof(msg_) + data_.size();

  // Send v1 error message when channel version is unknown.
  UInt8 version = channel->version();
  if (version == 0) {
    version = OFP_VERSION_1;
  }

  msg_.header_.setVersion(version);
  msg_.header_.setLength(msgLen);
  // Use the initialized value for xid.

  UInt16 savedCode = msg_.code_;
  UInt16 savedType = msg_.type_;
  OFPErrorCode errCode = OFPErrorCodeMake(savedType, savedCode);

  msg_.code_ = OFPErrorCodeToValue(version, errCode);
  msg_.type_ = OFPErrorTypeToValue(version, errCode);

  channel->write(&msg_, sizeof(msg_));
  channel->write(data_.data(), data_.size());
  channel->flush();

  msg_.code_ = savedCode;
  msg_.type_ = savedType;
}
