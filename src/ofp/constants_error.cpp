// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/constants_error.h"
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

static const ErrorType sErrorTypesV1[] = {
    {V1_OFPET_HELLO_FAILED, OFPET_HELLO_FAILED},
    {V1_OFPET_BAD_REQUEST, OFPET_BAD_REQUEST},
    {V1_OFPET_BAD_ACTION, OFPET_BAD_ACTION},
    {V1_OFPET_FLOW_MOD_FAILED, OFPET_FLOW_MOD_FAILED},
    {V1_OFPET_PORT_MOD_FAILED, OFPET_PORT_MOD_FAILED},
    {V1_OFPET_QUEUE_OP_FAILED, OFPET_QUEUE_OP_FAILED}};

static const ErrorCode sErrorFlowModFailedV1[] = {
    {V1_OFPFMFC_ALL_TABLES_FULL, OFPFMFC_TABLE_FULL},
    {V1_OFPFMFC_OVERLAP, OFPFMFC_OVERLAP},
    {V1_OFPFMFC_EPERM, OFPFMFC_EPERM},
    {V1_OFPFMFC_BAD_EMERG_TIMEOUT, OFPFMFC_BAD_TIMEOUT},
    {V1_OFPFMFC_BAD_COMMAND, OFPFMFC_BAD_COMMAND},
    {V1_OFPFMFC_UNSUPPORTED, OFPFMFC_UNSUPPORTED},
};

const OFPErrorType kMaxErrorType = OFPET_TABLE_FEATURES_FAILED;

// Given the actual packet values in an error message, determine the correct
// OFPErrorType enum value and return it. If the version and type do not
// correspond to a defined OFPErrorType value, return the original type value
// with the high bit set (OFPET_UNKNOWN_FLAG).
OFPErrorType ofp::OFPErrorTypeFromValue(UInt8 version, UInt16 type,
                                        UInt16 code) {
  // If the type is the experimenter type (0xffff), always return
  // OFPET_EXPERIMENTER regardless of the version.
  if (type == OFPET_EXPERIMENTER) {
    return OFPET_EXPERIMENTER;
  }

  if (version == OFP_VERSION_1) {
    // coverity[mixed_enums] (Coverity infers type is an OFPErrorType)
    for (const auto &i : sErrorTypesV1) {
      if (type == i.v1ErrType)
        return i.type;
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
UInt16 ofp::OFPErrorTypeToValue(UInt8 version, OFPErrorCode code) {
  OFPErrorType type = OFPErrorCodeGetType(code);

  if (type == OFPET_EXPERIMENTER) {
    return OFPET_EXPERIMENTER;
  }

  if (type & OFPET_UNKNOWN_FLAG) {
    return type & ~OFPET_UNKNOWN_FLAG;
  }

  if (version == OFP_VERSION_1) {
    for (const auto &i : sErrorTypesV1) {
      if (type == i.type)
        return i.v1ErrType;
    }
    log_warning("Unknown type enum in Error message:", static_cast<int>(type));
  } else if (code == OFPFMFC_UNSUPPORTED) {
    return OFPET_BAD_ACTION;
  }

  return type;
}

OFPErrorCode ofp::OFPErrorCodeFromValue(UInt8 version, UInt16 type,
                                        UInt16 code) {
  if (type == OFPET_EXPERIMENTER) {
    return OFPErrorCodeMake(type, code);
  }

  if (version == OFP_VERSION_1) {
    // coverity[mixed_enums] (Coverity infers type is an OFPErrorType)
    if (type == V1_OFPET_FLOW_MOD_FAILED) {
      for (auto i : sErrorFlowModFailedV1) {
        if (code == i.v1ErrCode)
          return i.code;
      }
    }

    OFPErrorType typeV1 = OFPErrorTypeFromValue(version, type, code);
    return OFPErrorCodeMake(typeV1, code);
  }

  return OFPErrorCodeMake(type, code);
}

UInt16 ofp::OFPErrorCodeToValue(UInt8 version, OFPErrorCode code) {
  if (version == OFP_VERSION_1) {
    for (auto i : sErrorFlowModFailedV1) {
      if (code == i.code) {
        return i.v1ErrCode;
      }
    }
  } else if (code == OFPFMFC_UNSUPPORTED) {
    code = OFPBAC_UNSUPPORTED_ORDER;
  }

  return (code & 0xffff);
}
