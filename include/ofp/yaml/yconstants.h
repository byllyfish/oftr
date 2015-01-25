// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YCONSTANTS_H_
#define OFP_YAML_YCONSTANTS_H_

#include "ofp/constants.h"
#include "ofp/yaml/enumconverter.h"
#include "ofp/yaml/ycontext.h"

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
#define OFP_YAML_BITCASE_V1(prefix, name) io.bitSetCase(value, #name, ofp::prefix##name##_V1)
#define OFP_YAML_MASKEDBITCASE(prefix, name, mask)  io.maskedBitSetCase(value, #name, ofp::prefix##name, ofp::mask)

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


template <>
struct ScalarBitSetTraits<ofp::OFPActionTypeFlags> {
  static void bitset(IO &io, ofp::OFPActionTypeFlags &value) {
    OFP_YAML_BITCASE(OFPATF_, OUTPUT);
    OFP_YAML_BITCASE(OFPATF_, SET_VLAN_VID);
    OFP_YAML_BITCASE(OFPATF_, SET_VLAN_PCP);
    OFP_YAML_BITCASE_V1(OFPATF_, STRIP_VLAN);   // special v1-only action
    OFP_YAML_BITCASE(OFPATF_, SET_DL_SRC);
    OFP_YAML_BITCASE(OFPATF_, SET_DL_DST);
    OFP_YAML_BITCASE(OFPATF_, SET_NW_SRC);
    OFP_YAML_BITCASE(OFPATF_, SET_NW_DST);
    OFP_YAML_BITCASE(OFPATF_, SET_NW_TOS);
    OFP_YAML_BITCASE(OFPATF_, SET_NW_ECN);
    OFP_YAML_BITCASE(OFPATF_, SET_TP_SRC);
    OFP_YAML_BITCASE(OFPATF_, SET_TP_DST);
    OFP_YAML_BITCASE_V1(OFPATF_, ENQUEUE);    // special v1-only action
    OFP_YAML_BITCASE(OFPATF_, COPY_TTL_OUT);
    OFP_YAML_BITCASE(OFPATF_, COPY_TTL_IN);
    OFP_YAML_BITCASE(OFPATF_, SET_MPLS_LABEL);
    OFP_YAML_BITCASE(OFPATF_, SET_MPLS_TC);
    OFP_YAML_BITCASE(OFPATF_, SET_MPLS_TTL);
    OFP_YAML_BITCASE(OFPATF_, DEC_MPLS_TTL);
    OFP_YAML_BITCASE(OFPATF_, PUSH_VLAN);
    OFP_YAML_BITCASE(OFPATF_, POP_VLAN);
    OFP_YAML_BITCASE(OFPATF_, PUSH_MPLS);
    OFP_YAML_BITCASE(OFPATF_, POP_MPLS);
    OFP_YAML_BITCASE(OFPATF_, SET_QUEUE);
    OFP_YAML_BITCASE(OFPATF_, GROUP);
    OFP_YAML_BITCASE(OFPATF_, SET_NW_TTL);
    OFP_YAML_BITCASE(OFPATF_, DEC_NW_TTL);
    OFP_YAML_BITCASE(OFPATF_, SET_FIELD);
    OFP_YAML_BITCASE(OFPATF_, PUSH_PBB);
    OFP_YAML_BITCASE(OFPATF_, POP_PBB);

    io.bitSetCaseOther(value, ofp::OFPATF_OTHER_FLAGS);
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPCapabilitiesFlags> {
  static void bitset(IO &io, ofp::OFPCapabilitiesFlags &value) {
    OFP_YAML_BITCASE(OFPC_, FLOW_STATS);
    OFP_YAML_BITCASE(OFPC_, TABLE_STATS);
    OFP_YAML_BITCASE(OFPC_, PORT_STATS);
    OFP_YAML_BITCASE(OFPC_, STP);
    OFP_YAML_BITCASE(OFPC_, GROUP_STATS);
    OFP_YAML_BITCASE(OFPC_, IP_REASM);
    OFP_YAML_BITCASE(OFPC_, QUEUE_STATS);
    OFP_YAML_BITCASE(OFPC_, ARP_MATCH_IP);
    OFP_YAML_BITCASE(OFPC_, PORT_BLOCKED);

    io.bitSetCaseOther(value, ofp::OFPC_OTHER_CAPABILITIES_FLAGS);
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPConfigFlags> {
  static void bitset(IO &io, ofp::OFPConfigFlags &value) {
    OFP_YAML_MASKEDBITCASE(OFPC_, FRAG_NORMAL, OFPC_FRAG_MASK);
    OFP_YAML_MASKEDBITCASE(OFPC_, FRAG_DROP, OFPC_FRAG_MASK);
    OFP_YAML_MASKEDBITCASE(OFPC_, FRAG_REASM, OFPC_FRAG_MASK);

    io.bitSetCaseOther(value, ofp::OFPC_OTHER_CONFIG_FLAGS);
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPPortFeaturesFlags> {
  static void bitset(IO &io, ofp::OFPPortFeaturesFlags &value) {
    OFP_YAML_BITCASE(OFPPF_, 10MB_HD);
    OFP_YAML_BITCASE(OFPPF_, 10MB_FD);
    OFP_YAML_BITCASE(OFPPF_, 100MB_HD);
    OFP_YAML_BITCASE(OFPPF_, 100MB_FD);
    OFP_YAML_BITCASE(OFPPF_, 1GB_HD);
    OFP_YAML_BITCASE(OFPPF_, 1GB_FD);
    OFP_YAML_BITCASE(OFPPF_, 10GB_FD);
    OFP_YAML_BITCASE(OFPPF_, 40GB_FD);
    OFP_YAML_BITCASE(OFPPF_, 100GB_FD);
    OFP_YAML_BITCASE(OFPPF_, 1TB_FD);
    OFP_YAML_BITCASE(OFPPF_, OTHER);
    OFP_YAML_BITCASE(OFPPF_, COPPER);
    OFP_YAML_BITCASE(OFPPF_, FIBER);
    OFP_YAML_BITCASE(OFPPF_, AUTONEG);
    OFP_YAML_BITCASE(OFPPF_, PAUSE);
    OFP_YAML_BITCASE(OFPPF_, PAUSE_ASYM);

    io.bitSetCaseOther(value, ofp::OFPPF_OTHER_FEATURES_FLAGS);
  }
};


template <>
struct ScalarBitSetTraits<ofp::OFPPortConfigFlags> {
  static void bitset(IO &io, ofp::OFPPortConfigFlags &value) {
    OFP_YAML_BITCASE(OFPPC_, PORT_DOWN);
    OFP_YAML_BITCASE(OFPPC_, NO_STP);
    OFP_YAML_BITCASE(OFPPC_, NO_RECV);
    OFP_YAML_BITCASE(OFPPC_, NO_RECV_STP);
    OFP_YAML_BITCASE(OFPPC_, NO_FLOOD);
    OFP_YAML_BITCASE(OFPPC_, NO_FWD);
    OFP_YAML_BITCASE(OFPPC_, NO_PACKET_IN);

    io.bitSetCaseOther(value, ofp::OFPPC_OTHER_CONFIG_FLAGS);
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPPortStateFlags> {
  static void bitset(IO &io, ofp::OFPPortStateFlags &value) {
    OFP_YAML_BITCASE(OFPPS_, LINK_DOWN);
    OFP_YAML_BITCASE(OFPPS_, BLOCKED);
    OFP_YAML_BITCASE(OFPPS_, LIVE);

    if (ofp::yaml::GetVersionFromContext(io) <= ofp::OFP_VERSION_1) {
      OFP_YAML_MASKEDBITCASE(OFPPS_, STP_LISTEN, OFPPS_STP_MASK);
      OFP_YAML_MASKEDBITCASE(OFPPS_, STP_LEARN, OFPPS_STP_MASK);
      OFP_YAML_MASKEDBITCASE(OFPPS_, STP_FORWARD, OFPPS_STP_MASK);
      OFP_YAML_MASKEDBITCASE(OFPPS_, STP_BLOCK, OFPPS_STP_MASK);
      io.bitSetCaseOther(value, ofp::OFPPS_OTHER_STATE_FLAGS_V1);
    } else {
      io.bitSetCaseOther(value, ofp::OFPPS_OTHER_STATE_FLAGS);
    }
  }
};

#undef OFP_YAML_BITCASE
#undef OFP_YAML_BITCASE_V1
#undef OFP_YAML_MASKEDBITCASE
#undef OFP_YAML_ENUMCASE

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YCONSTANTS_H_
