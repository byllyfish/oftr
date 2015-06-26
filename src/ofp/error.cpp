// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/error.h"
#include "ofp/message.h"
#include "ofp/writable.h"
#include "ofp/log.h"

using namespace ofp;

// In OpenFlow 1.0, the error types are assigned different values.
enum V1_ErrorTypes : UInt16 {
  V1_OFPET_HELLO_FAILED = 0,
  V1_OFPET_BAD_REQUEST = 1,
  V1_OFPET_BAD_ACTION = 2,
  V1_OFPET_FLOW_MOD_FAILED = 3,
  V1_OFPET_PORT_MOD_FAILED = 4,
  V1_OFPET_QUEUE_OP_FAILED = 5
};

// In OpenFlow 1.0, the OFPET_FLOW_MOD_FAILED error codes are assigned different
// values.
enum V1_FlowModFailedCodes : UInt16 {
  V1_OFPFMFC_ALL_TABLES_FULL = 0,
  V1_OFPFMFC_OVERLAP = 1,
  V1_OFPFMFC_EPERM = 2,
  V1_OFPFMFC_BAD_EMERG_TIMEOUT = 3,
  V1_OFPFMFC_BAD_COMMAND = 4,
  V1_OFPFMFC_UNSUPPORTED = 5,
};

struct ErrorType {
  UInt16 v1ErrType;
  OFPErrorType type;
};

OFP_BEGIN_IGNORE_PADDING

struct ErrorCode {
  UInt16 v1ErrCode;
  OFPErrorCode code;
};

OFP_END_IGNORE_PADDING

static ErrorType sErrorTypesV1[] = {
    {V1_OFPET_HELLO_FAILED, OFPET_HELLO_FAILED},
    {V1_OFPET_BAD_REQUEST, OFPET_BAD_REQUEST},
    {V1_OFPET_BAD_ACTION, OFPET_BAD_ACTION},
    {V1_OFPET_FLOW_MOD_FAILED, OFPET_FLOW_MOD_FAILED},
    {V1_OFPET_PORT_MOD_FAILED, OFPET_PORT_MOD_FAILED},
    {V1_OFPET_QUEUE_OP_FAILED, OFPET_QUEUE_OP_FAILED}};

static ErrorCode sErrorFlowModFailedV1[] = {
    {V1_OFPFMFC_ALL_TABLES_FULL, OFPFMFC_TABLE_FULL},
    {V1_OFPFMFC_OVERLAP, OFPFMFC_OVERLAP},
    {V1_OFPFMFC_EPERM, OFPFMFC_EPERM},
    {V1_OFPFMFC_BAD_EMERG_TIMEOUT, OFPFMFC_BAD_TIMEOUT},
    {V1_OFPFMFC_BAD_COMMAND, OFPFMFC_BAD_COMMAND},
    {V1_OFPFMFC_UNSUPPORTED, OFPBAC_UNSUPPORTED_ORDER},
};

const OFPErrorType kMaxErrorType = OFPET_TABLE_FEATURES_FAILED;

// Given the actual packet values in an error message, determine the correct
// OFPErrorType enum value and return it. If the version and type do not
// correspond to a defined OFPErrorType value, return the original type value
// with the high bit set (OFPET_UNKNOWN_FLAG). If the type is the experimenter
// type (0xffff), we will always return OFPET_EXPERIMENTER regardless of the
// version.

static OFPErrorType typeToEnum(UInt8 version, UInt16 type, UInt16 code) {
  if (type == OFPET_EXPERIMENTER) {
    return OFPET_EXPERIMENTER;
  }

  if (version == OFP_VERSION_1) {
    // Map the OFPFMFC_UNSUPPORTED error code to OFPET_BAD_ACTION type.
    // coverity[mixed_enums] (Coverity infers type is an OFPErrorType)
    if (type == V1_OFPET_FLOW_MOD_FAILED) {
      return code == V1_OFPFMFC_UNSUPPORTED ? OFPET_BAD_ACTION
                                            : OFPET_FLOW_MOD_FAILED;
    }

    for (size_t i = 0; i < ArrayLength(sErrorTypesV1); ++i) {
      if (type == sErrorTypesV1[i].v1ErrType)
        return sErrorTypesV1[i].type;
    }
    type |= OFPET_UNKNOWN_FLAG;

  } else if (type > kMaxErrorType) {
    type |= OFPET_UNKNOWN_FLAG;
  }

  return static_cast<OFPErrorType>(type);
}

// Given an OFPErrorType and a version, determine the correct error type value
// to use in an Error message. If the error type is OFPET_EXPERIMENTER, we
// always map this to 0xffff regardless of the version. If the error type has
// the high bit set (OFPET_UNKNOWN_FLAG), we return the type value with the
// high bit cleared.

static UInt16 enumToType(UInt8 version, OFPErrorType type) {
  if (type == OFPET_EXPERIMENTER) {
    return OFPET_EXPERIMENTER;
  }

  if (type & OFPET_UNKNOWN_FLAG) {
    return type & ~OFPET_UNKNOWN_FLAG;
  }

  if (version == OFP_VERSION_1) {
    for (size_t i = 0; i < ArrayLength(sErrorTypesV1); ++i) {
      if (type == sErrorTypesV1[i].type)
        return sErrorTypesV1[i].v1ErrType;
    }
    log::warning("Unknown type enum in Error message:", static_cast<int>(type));
  }

  return type;
}

static UInt16 enumToCode(UInt8 version, OFPErrorCode code) {
  if (version == OFP_VERSION_1) {
    for (size_t i = 0; i < ArrayLength(sErrorFlowModFailedV1); ++i) {
      if (code == sErrorFlowModFailedV1[i].code) {
        return sErrorFlowModFailedV1[i].v1ErrCode;
      }
    }
  }

  return (code & 0xffff);
}

static OFPErrorCode codeToEnum(UInt8 version, UInt16 type, UInt16 code) {
  if (type == OFPET_EXPERIMENTER) {
    return OFPErrorCodeMake(type, code);
  }

  if (version == OFP_VERSION_1) {
    // coverity[mixed_enums] (Coverity infers type is an OFPErrorType)
    if (type == V1_OFPET_BAD_ACTION && code == V1_OFPFMFC_UNSUPPORTED) {
      return OFPBAC_UNSUPPORTED_ORDER;
    } else if (type == V1_OFPET_FLOW_MOD_FAILED) {
      for (size_t i = 0; i < ArrayLength(sErrorFlowModFailedV1); ++i) {
        if (code == sErrorFlowModFailedV1[i].v1ErrCode)
          return sErrorFlowModFailedV1[i].code;
      }
    }

    OFPErrorType typeV1 = typeToEnum(version, type, code);
    return OFPErrorCodeMake(typeV1, code);
  }

  return OFPErrorCodeMake(type, code);
}

ByteRange Error::errorData() const {
  return ByteRange{BytePtr(this) + sizeof(Error),
                   header_.length() - sizeof(Error)};
}

OFPErrorType Error::errorType() const {
  // The error type values differ in meaning between protocol version 1 and
  // later versions. A type of EXPERIMENTER will always be honored.

  return typeToEnum(msgHeader()->version(), type_, code_);
}

OFPErrorCode Error::errorCode() const {
  return codeToEnum(msgHeader()->version(), type_, code_);
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
      std::stringstream ss;
      ss << "Type: " << msgType;
      if (msgType == OFPT_MULTIPART_REQUEST ||
          msgType == OFPT_MULTIPART_REPLY) {
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
  msg_.header_.setLength(UInt16_narrow_cast(msgLen));
  msg_.header_.setXid(channel->nextXid());

  UInt16 savedCode = msg_.code_;
  UInt16 savedType = msg_.type_;

  msg_.code_ = enumToCode(version, OFPErrorCodeMake(savedType, savedCode));
  msg_.type_ = enumToType(version, static_cast<OFPErrorType>(savedType));

  channel->write(&msg_, sizeof(msg_));
  channel->write(data_.data(), data_.size());
  channel->flush();

  msg_.code_ = savedCode;
  msg_.type_ = savedType;
}
