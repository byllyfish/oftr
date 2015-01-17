// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YCONSTANTS_H_
#define OFP_YAML_YCONSTANTS_H_

#include "ofp/constants.h"
#include "ofp/yaml/enumconverter.h"

namespace llvm {
namespace yaml {

#define OFP_YAML_ENUMCASE(val) io.enumCase(value, #val, ofp::val)

template <>
struct ScalarEnumerationTraits<ofp::OFPType> {
  static void enumeration(IO &io, ofp::OFPType &value) {
    OFP_YAML_ENUMCASE(OFPT_HELLO);
    OFP_YAML_ENUMCASE(OFPT_ERROR);
    OFP_YAML_ENUMCASE(OFPT_ECHO_REQUEST);
    OFP_YAML_ENUMCASE(OFPT_ECHO_REPLY);
    OFP_YAML_ENUMCASE(OFPT_EXPERIMENTER);
    OFP_YAML_ENUMCASE(OFPT_FEATURES_REQUEST);
    OFP_YAML_ENUMCASE(OFPT_FEATURES_REPLY);
    OFP_YAML_ENUMCASE(OFPT_GET_CONFIG_REQUEST);
    OFP_YAML_ENUMCASE(OFPT_GET_CONFIG_REPLY);
    OFP_YAML_ENUMCASE(OFPT_SET_CONFIG);
    OFP_YAML_ENUMCASE(OFPT_PACKET_IN);
    OFP_YAML_ENUMCASE(OFPT_FLOW_REMOVED);
    OFP_YAML_ENUMCASE(OFPT_PORT_STATUS);
    OFP_YAML_ENUMCASE(OFPT_PACKET_OUT);
    OFP_YAML_ENUMCASE(OFPT_FLOW_MOD);
    OFP_YAML_ENUMCASE(OFPT_GROUP_MOD);
    OFP_YAML_ENUMCASE(OFPT_PORT_MOD);
    OFP_YAML_ENUMCASE(OFPT_TABLE_MOD);
    OFP_YAML_ENUMCASE(OFPT_MULTIPART_REQUEST);
    OFP_YAML_ENUMCASE(OFPT_MULTIPART_REPLY);
    OFP_YAML_ENUMCASE(OFPT_BARRIER_REQUEST);
    OFP_YAML_ENUMCASE(OFPT_BARRIER_REPLY);
    OFP_YAML_ENUMCASE(OFPT_QUEUE_GET_CONFIG_REQUEST);
    OFP_YAML_ENUMCASE(OFPT_QUEUE_GET_CONFIG_REPLY);
    OFP_YAML_ENUMCASE(OFPT_ROLE_REQUEST);
    OFP_YAML_ENUMCASE(OFPT_ROLE_REPLY);
    OFP_YAML_ENUMCASE(OFPT_GET_ASYNC_REQUEST);
    OFP_YAML_ENUMCASE(OFPT_GET_ASYNC_REPLY);
    OFP_YAML_ENUMCASE(OFPT_SET_ASYNC);
    OFP_YAML_ENUMCASE(OFPT_METER_MOD);
  }
};

template <>
struct ScalarEnumerationTraits<ofp::OFPMultipartType> {
  static void enumeration(IO &io, ofp::OFPMultipartType &value) {
    OFP_YAML_ENUMCASE(OFPMP_DESC);
    OFP_YAML_ENUMCASE(OFPMP_FLOW);
    OFP_YAML_ENUMCASE(OFPMP_AGGREGATE);
    OFP_YAML_ENUMCASE(OFPMP_TABLE);
    OFP_YAML_ENUMCASE(OFPMP_PORT_STATS);
    OFP_YAML_ENUMCASE(OFPMP_QUEUE);
    OFP_YAML_ENUMCASE(OFPMP_GROUP);
    OFP_YAML_ENUMCASE(OFPMP_GROUP_DESC);
    OFP_YAML_ENUMCASE(OFPMP_GROUP_FEATURES);
    OFP_YAML_ENUMCASE(OFPMP_METER);
    OFP_YAML_ENUMCASE(OFPMP_METER_CONFIG);
    OFP_YAML_ENUMCASE(OFPMP_METER_FEATURES);
    OFP_YAML_ENUMCASE(OFPMP_TABLE_FEATURES);
    OFP_YAML_ENUMCASE(OFPMP_PORT_DESC);
    OFP_YAML_ENUMCASE(OFPMP_EXPERIMENTER);
  }
};

template <>
struct ScalarEnumerationTraits<ofp::OFPInstructionType> {
  static void enumeration(IO &io, ofp::OFPInstructionType &value) {
    OFP_YAML_ENUMCASE(OFPIT_GOTO_TABLE);
    OFP_YAML_ENUMCASE(OFPIT_WRITE_METADATA);
    OFP_YAML_ENUMCASE(OFPIT_WRITE_ACTIONS);
    OFP_YAML_ENUMCASE(OFPIT_APPLY_ACTIONS);
    OFP_YAML_ENUMCASE(OFPIT_CLEAR_ACTIONS);
    OFP_YAML_ENUMCASE(OFPIT_METER);
    OFP_YAML_ENUMCASE(OFPIT_EXPERIMENTER);
  }
};

#define YAML_ENUM_CONVERTER(ConverterType, EnumType, FormatExpr)  \
template <> \
struct ScalarTraits<EnumType> { \
  static ConverterType<EnumType> converter; \
  static void output(const EnumType &value, void *ctxt, \
                     llvm::raw_ostream &out) { \
    llvm::StringRef scalar; \
    if (converter.convert(value, &scalar)) { \
      out << scalar; \
    } else { \
      out << FormatExpr; \
    } \
  } \
  static StringRef input(StringRef scalar, void *ctxt, \
                         EnumType &value) { \
    if (converter.convert(scalar, &value)) { \
      return ""; \
    } \
    return "Invalid enumerated constant."; \
  } \
  static bool mustQuote(StringRef) { return false; } \
};

#if 0
#define YAML_ENUM_CONVERTER_16(EnumType)  \
static_assert(sizeof(EnumType) == 2, "Expected 16-bit enum."); \
template <> \
struct ScalarTraits<EnumType> { \
  static ofp::yaml::EnumConverter<EnumType> converter; \
  static void output(const EnumType &value, void *ctxt, \
                     llvm::raw_ostream &out) { \
    llvm::StringRef scalar; \
    if (converter.convert(value, &scalar)) { \
      out << scalar; \
    } else { \
      out << format("0x%04X", value); \
    } \
  } \
  static StringRef input(StringRef scalar, void *ctxt, \
                         EnumType &value) { \
    if (converter.convert(scalar, &value)) { \
      return ""; \
    } \
    return "Invalid enumerated constant."; \
  } \
  static bool mustQuote(StringRef) { return false; } \
};
#endif //0

template <>
struct ScalarTraits<ofp::OFPPacketInReason> {
  static ofp::yaml::EnumConverter<ofp::OFPPacketInReason> converter;

  static void output(const ofp::OFPPacketInReason &value, void *ctxt,
                     llvm::raw_ostream &out) {
    llvm::StringRef scalar;
    if (converter.convert(value, &scalar)) {
      out << scalar;
    } else {
      out << format("0x%02X", value);
    }
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::OFPPacketInReason &value) {
    if (converter.convert(scalar, &value)) {
      return "";
    }

    return "Invalid enumerated constant.";
  }

  static bool mustQuote(StringRef) { return false; }
};

template <>
struct ScalarTraits<ofp::OFPFlowModCommand> {
  static ofp::yaml::EnumConverter<ofp::OFPFlowModCommand> converter;

  static void output(const ofp::OFPFlowModCommand &value, void *ctxt,
                     llvm::raw_ostream &out) {
    llvm::StringRef scalar;
    if (converter.convert(value, &scalar)) {
      out << scalar;
    } else {
      out << format("0x%02X", value);
    }
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::OFPFlowModCommand &value) {
    if (converter.convert(scalar, &value)) {
      return "";
    }

    return "Invalid enumerated constant.";
  }

  static bool mustQuote(StringRef) { return false; }
};

template <>
struct ScalarEnumerationTraits<ofp::OFPMeterBandType> {
  static void enumeration(IO &io, ofp::OFPMeterBandType &value) {
    OFP_YAML_ENUMCASE(OFPMBT_DROP);
    OFP_YAML_ENUMCASE(OFPMBT_DSCP_REMARK);
    OFP_YAML_ENUMCASE(OFPMBT_EXPERIMENTER);
  }
};

YAML_ENUM_CONVERTER(ofp::yaml::EnumConverter, ofp::OFPFlowRemovedReason, format("0x%02X", value))

YAML_ENUM_CONVERTER(ofp::yaml::EnumConverter, ofp::OFPErrorType, format("0x%04X", value))

//YAML_ENUM_CONVERTER(ofp::yaml::EnumConverterSparse, ofp::OFPErrorCode, format("0x%08X", value))


template <>
struct ScalarTraits<ofp::OFPErrorCode> {
  static ofp::yaml::EnumConverterSparse<ofp::OFPErrorCode> converter;

  static void output(const ofp::OFPErrorCode &value, void *ctxt,
                     llvm::raw_ostream &out) {
    llvm::StringRef scalar;
    if (converter.convert(value, &scalar)) {
      out << scalar;
    } else {
      out << format("0x%04X", value & 0xffff);
    }
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::OFPErrorCode &value) {
    if (converter.convert(scalar, &value)) {
      return "";
    }
    if (ofp::yaml::ParseUnsignedInteger(scalar, &value)) {
      value = static_cast<ofp::OFPErrorCode>(value | ofp::OFPEC_UNKNOWN_FLAG);
      return "";
    }
    return "Invalid enumerated constant.";
  }

  static bool mustQuote(StringRef) { return false; }
};

#define OFP_YAML_BITCASE(prefix, name)    io.bitSetCase(value, #name, ofp::prefix##name)

template <>
struct ScalarBitSetTraits<ofp::OFPFlowModFlags> {
  static void bitset(IO &io, ofp::OFPFlowModFlags &value) {
    OFP_YAML_BITCASE(OFPFF_, SEND_FLOW_REM);
    OFP_YAML_BITCASE(OFPFF_, CHECK_OVERLAP);
    OFP_YAML_BITCASE(OFPFF_, RESET_COUNTS);
    OFP_YAML_BITCASE(OFPFF_, NO_PKT_COUNTS);
    OFP_YAML_BITCASE(OFPFF_, NO_BYT_COUNTS);

    io.bitSetCaseOther(value, ofp::OFPFF_OTHER_FLAGS);
  }
};


#undef OFP_YAML_BITCASE
#undef OFP_YAML_ENUMCASE

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YCONSTANTS_H_
