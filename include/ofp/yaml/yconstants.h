// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YCONSTANTS_H_
#define OFP_YAML_YCONSTANTS_H_

#include "ofp/byteorder.h"
#include "ofp/messagetype.h"
#include "ofp/yaml/enumconverter.h"
#include "ofp/yaml/seterror.h"
#include "ofp/yaml/ycontext.h"

namespace ofp {
namespace yaml {

template <class Type>
std::string AllFlags() {
  std::string result;
  llvm::raw_string_ostream rs{result};
  llvm::yaml::Output out{rs};
  Type val;
  std::memset(&val, 0xFF, sizeof(val));
  llvm::yaml::ScalarBitSetTraits<Type>::bitset(out, val);
  return rs.str();
}

llvm::StringRef ParseMessageType(llvm::StringRef name, void *ctxt,
                                 MessageType &msgType);

}  // namespace yaml
}  // namespace ofp

namespace llvm {
namespace yaml {

// Support ScalarBitSetTraits for Big<Enum> types.
template <typename T>
typename std::enable_if<has_ScalarBitSetTraits<T>::value, void>::type yamlize(
    IO &io, ofp::Big<T> &Val, bool, EmptyContext &Ctx) {
  bool DoClear;
  if (io.beginBitSetScalar(DoClear)) {
    T value = Val;
    if (DoClear)
      value = static_cast<T>(0);
    ScalarBitSetTraits<T>::bitset(io, value);
    Val = value;
    io.endBitSetScalar();
  }
}

// Support ScalarTraits for Big<Enum> types.
template <typename T>
typename std::enable_if<has_ScalarTraits<T>::value && std::is_enum<T>::value,
                        void>::type
yamlize(IO &io, ofp::Big<T> &Val, bool ignore, EmptyContext &Ctx) {
  T value = Val;
  yamlize(io, value, ignore, Ctx);
  Val = value;
}

#define YAML_ENUM_CONVERTER(ConverterType, EnumType, FormatExpr)            \
  template <>                                                               \
  struct ScalarTraits<EnumType> {                                           \
    static const ConverterType<EnumType> converter;                         \
    static void output(const EnumType &value, void *ctxt,                   \
                       llvm::raw_ostream &out) {                            \
      llvm::StringRef scalar;                                               \
      if (converter.convert(value, &scalar)) {                              \
        out << scalar;                                                      \
      } else {                                                              \
        out << FormatExpr;                                                  \
      }                                                                     \
    }                                                                       \
    static StringRef input(StringRef scalar, void *ctxt, EnumType &value) { \
      if (converter.convert(scalar, &value))                                \
        return "";                                                          \
      return ofp::yaml::SetEnumError(ctxt, scalar, converter.listAll());    \
    }                                                                       \
    static QuotingType mustQuote(StringRef) { return QuotingType::None; }   \
  };

YAML_ENUM_CONVERTER(ofp::yaml::EnumConverter, ofp::OFPType,
                    format("0x%02X", value))
YAML_ENUM_CONVERTER(ofp::yaml::EnumConverter, ofp::OFPMultipartType,
                    format("0x%04X", value))
YAML_ENUM_CONVERTER(ofp::yaml::EnumConverter, ofp::OFPInstructionType,
                    format("0x%04X", value))
YAML_ENUM_CONVERTER(ofp::yaml::EnumConverter, ofp::OFPMeterBandType,
                    format("0x%04X", value))
YAML_ENUM_CONVERTER(ofp::yaml::EnumConverter, ofp::OFPPacketInReason,
                    format("0x%02X", value))
YAML_ENUM_CONVERTER(ofp::yaml::EnumConverter, ofp::OFPFlowModCommand,
                    format("0x%02X", value))
YAML_ENUM_CONVERTER(ofp::yaml::EnumConverter, ofp::OFPPortStatusReason,
                    format("0x%02X", value))
YAML_ENUM_CONVERTER(ofp::yaml::EnumConverter, ofp::OFPFlowRemovedReason,
                    format("0x%02X", value))
YAML_ENUM_CONVERTER(ofp::yaml::EnumConverter, ofp::OFPTableStatusReason,
                    format("0x%02X", value))
YAML_ENUM_CONVERTER(ofp::yaml::EnumConverter, ofp::OFPControllerRole,
                    format("0x%08X", value))
YAML_ENUM_CONVERTER(ofp::yaml::EnumConverter, ofp::OFPMeterModCommand,
                    format("0x%04X", value))
YAML_ENUM_CONVERTER(ofp::yaml::EnumConverter, ofp::OFPGroupModCommand,
                    format("0x%04X", value))
YAML_ENUM_CONVERTER(ofp::yaml::EnumConverter, ofp::OFPGroupType,
                    format("0x%02X", value))
YAML_ENUM_CONVERTER(ofp::yaml::EnumConverter, ofp::OFPErrorType,
                    format("0x%04X", value))
YAML_ENUM_CONVERTER(ofp::yaml::EnumConverter, ofp::OFPFlowUpdateEvent,
                    format("0x%04X", value))
YAML_ENUM_CONVERTER(ofp::yaml::EnumConverter, ofp::OFPFlowMonitorCommand,
                    format("0x%02X", value))
YAML_ENUM_CONVERTER(ofp::yaml::EnumConverter, ofp::OFPBundleCtrlType,
                    format("0x%04X", value))
YAML_ENUM_CONVERTER(ofp::yaml::EnumConverter, ofp::OFPExperimenterPropertyType,
                    format("0x%04X", value))
YAML_ENUM_CONVERTER(ofp::yaml::EnumConverter, ofp::OFPRoleStatusReason,
                    format("0x%02X", value))
YAML_ENUM_CONVERTER(ofp::yaml::EnumConverterSparse, ofp::OFPAsyncConfigProperty,
                    format("0x%02X", value))

template <>
struct ScalarTraits<ofp::OFPErrorCode> {
  static const ofp::yaml::EnumConverterSparse<ofp::OFPErrorCode> converter;

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
    return ofp::yaml::SetEnumError(ctxt, scalar, converter.listAll());
  }

  static QuotingType mustQuote(StringRef) { return QuotingType::None; }
};

template <>
struct ScalarTraits<ofp::MessageType> {
  static void output(const ofp::MessageType &value, void *ctxt,
                     llvm::raw_ostream &out) {
    if (value.type_ == ofp::OFPT_MULTIPART_REQUEST) {
      ScalarTraits<ofp::OFPMultipartType>::output(value.subtype_, ctxt, out);
      out << "_REQUEST";

    } else if (value.type_ == ofp::OFPT_MULTIPART_REPLY) {
      ScalarTraits<ofp::OFPMultipartType>::output(value.subtype_, ctxt, out);
      out << "_REPLY";

    } else {
      ScalarTraits<ofp::OFPType>::output(value.type_, ctxt, out);
    }
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::MessageType &value) {
    return ofp::yaml::ParseMessageType(scalar, ctxt, value);
  }

  static QuotingType mustQuote(StringRef) { return QuotingType::None; }
};

#define OFP_YAML_BITCASE(prefix, name) \
  io.bitSetCase(value, #name, ofp::prefix##name)
#define OFP_YAML_BITCASE_V1(prefix, name) \
  io.bitSetCase(value, #name, ofp::prefix##name##_V1)
#define OFP_YAML_MASKEDBITCASE(prefix, name, mask) \
  io.maskedBitSetCase(value, #name, ofp::prefix##name, ofp::mask)

template <>
struct ScalarBitSetTraits<ofp::OFPFlowModFlags> {
  static void bitset(IO &io, ofp::OFPFlowModFlags &value) {
    OFP_YAML_BITCASE(OFPFF_, SEND_FLOW_REM);
    OFP_YAML_BITCASE(OFPFF_, CHECK_OVERLAP);
    OFP_YAML_BITCASE(OFPFF_, RESET_COUNTS);
    OFP_YAML_BITCASE(OFPFF_, NO_PKT_COUNTS);
    OFP_YAML_BITCASE(OFPFF_, NO_BYT_COUNTS);

    io.bitSetCaseOther(value, ofp::OFPFF_OTHER_FLAGS);

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(io, val,
                              ofp::yaml::AllFlags<ofp::OFPFlowModFlags>());
    }
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPActionTypeFlags> {
  static void bitset(IO &io, ofp::OFPActionTypeFlags &value) {
    OFP_YAML_BITCASE(OFPATF_, OUTPUT);
    OFP_YAML_BITCASE(OFPATF_, SET_VLAN_VID);
    OFP_YAML_BITCASE(OFPATF_, SET_VLAN_PCP);
    OFP_YAML_BITCASE_V1(OFPATF_, STRIP_VLAN);  // special v1-only action
    OFP_YAML_BITCASE(OFPATF_, SET_DL_SRC);
    OFP_YAML_BITCASE(OFPATF_, SET_DL_DST);
    OFP_YAML_BITCASE(OFPATF_, SET_NW_SRC);
    OFP_YAML_BITCASE(OFPATF_, SET_NW_DST);
    OFP_YAML_BITCASE(OFPATF_, SET_NW_TOS);
    OFP_YAML_BITCASE(OFPATF_, SET_NW_ECN);
    OFP_YAML_BITCASE(OFPATF_, SET_TP_SRC);
    OFP_YAML_BITCASE(OFPATF_, SET_TP_DST);
    OFP_YAML_BITCASE_V1(OFPATF_, ENQUEUE);  // special v1-only action
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

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(io, val,
                              ofp::yaml::AllFlags<ofp::OFPActionTypeFlags>());
    }
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

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(io, val,
                              ofp::yaml::AllFlags<ofp::OFPCapabilitiesFlags>());
    }
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPConfigFlags> {
  static void bitset(IO &io, ofp::OFPConfigFlags &value) {
    OFP_YAML_MASKEDBITCASE(OFPC_, FRAG_NORMAL, OFPC_FRAG_MASK);
    OFP_YAML_MASKEDBITCASE(OFPC_, FRAG_DROP, OFPC_FRAG_MASK);
    OFP_YAML_MASKEDBITCASE(OFPC_, FRAG_REASM, OFPC_FRAG_MASK);

    io.bitSetCaseOther(value, ofp::OFPC_OTHER_CONFIG_FLAGS);

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(io, val, "FRAG_NORMAL,FRAG_DROP,FRAG_REASM");
    }
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

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(io, val,
                              ofp::yaml::AllFlags<ofp::OFPPortFeaturesFlags>());
    }
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPOpticalPortFeaturesFlags> {
  static void bitset(IO &io, ofp::OFPOpticalPortFeaturesFlags &value) {
    OFP_YAML_BITCASE(OFPOPF_, RX_TUNE);
    OFP_YAML_BITCASE(OFPOPF_, TX_TUNE);
    OFP_YAML_BITCASE(OFPOPF_, TX_PWR);
    OFP_YAML_BITCASE(OFPOPF_, USE_FREQ);

    io.bitSetCaseOther(value, ofp::OFPOPF_OTHER_FEATURES_FLAGS);

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(
          io, val, ofp::yaml::AllFlags<ofp::OFPOpticalPortFeaturesFlags>());
    }
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

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(io, val,
                              ofp::yaml::AllFlags<ofp::OFPPortConfigFlags>());
    }
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

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      std::string vals = ofp::yaml::AllFlags<ofp::OFPPortStateFlags>();
      if (ofp::yaml::GetVersionFromContext(io) <= ofp::OFP_VERSION_1) {
        vals += "STP_LISTEN,STP_LEARN,STP_FORWARD";
      }
      ofp::yaml::SetFlagError(io, val, vals);
    }
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPMultipartFlags> {
  static void bitset(IO &io, ofp::OFPMultipartFlags &value) {
    OFP_YAML_BITCASE(OFPMPF_, MORE);

    io.bitSetCaseOther(value, ofp::OFPMPF_OTHER_MULTIPART_FLAGS);

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(io, val,
                              ofp::yaml::AllFlags<ofp::OFPMultipartFlags>());
    }
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPMessageFlags> {
  static void bitset(IO &io, ofp::OFPMessageFlags &value) {
    OFP_YAML_BITCASE(OFP_, MORE);
    OFP_YAML_BITCASE(OFP_, NO_FLUSH);
    OFP_YAML_BITCASE(OFP_, NO_ALERT);
    OFP_YAML_BITCASE(OFP_, REPLIED);

    io.bitSetCaseOther(value, ofp::OFP_OTHER_MESSAGE_FLAGS);

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(io, val,
                              ofp::yaml::AllFlags<ofp::OFPMessageFlags>());
    }
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPMeterConfigFlags> {
  static void bitset(IO &io, ofp::OFPMeterConfigFlags &value) {
    OFP_YAML_BITCASE(OFPMCF_, KBPS);
    OFP_YAML_BITCASE(OFPMCF_, PKTPS);
    OFP_YAML_BITCASE(OFPMCF_, BURST);
    OFP_YAML_BITCASE(OFPMCF_, STATS);

    io.bitSetCaseOther(value, ofp::OFPMCF_OTHER_METER_CONFIG_FLAGS);

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(io, val,
                              ofp::yaml::AllFlags<ofp::OFPMeterConfigFlags>());
    }
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPMeterFlags> {
  static void bitset(IO &io, ofp::OFPMeterFlags &value) {
    OFP_YAML_BITCASE(OFPMF_, KBPS);
    OFP_YAML_BITCASE(OFPMF_, PKTPS);
    OFP_YAML_BITCASE(OFPMF_, BURST);
    OFP_YAML_BITCASE(OFPMF_, STATS);

    io.bitSetCaseOther(value, ofp::OFPMF_OTHER_METER_FLAGS);

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(io, val,
                              ofp::yaml::AllFlags<ofp::OFPMeterFlags>());
    }
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPPacketInFlags> {
  static void bitset(IO &io, ofp::OFPPacketInFlags &value) {
    OFP_YAML_BITCASE(OFPRF_, TABLE_MISS);
    OFP_YAML_BITCASE(OFPRF_, APPLY_ACTION);
    OFP_YAML_BITCASE(OFPRF_, INVALID_TTL);
    OFP_YAML_BITCASE(OFPRF_, ACTION_SET);
    OFP_YAML_BITCASE(OFPRF_, GROUP);
    OFP_YAML_BITCASE(OFPRF_, PACKET_OUT);

    io.bitSetCaseOther(value, ofp::OFPRF_OTHER_PACKET_IN_FLAGS);

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(io, val,
                              ofp::yaml::AllFlags<ofp::OFPPacketInFlags>());
    }
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPPortStatusFlags> {
  static void bitset(IO &io, ofp::OFPPortStatusFlags &value) {
    OFP_YAML_BITCASE(OFPPRF_, ADD);
    OFP_YAML_BITCASE(OFPPRF_, DELETE);
    OFP_YAML_BITCASE(OFPPRF_, MODIFY);

    io.bitSetCaseOther(value, ofp::OFPPRF_OTHER_PORTSTATUS_FLAGS);

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(io, val,
                              ofp::yaml::AllFlags<ofp::OFPPortStatusFlags>());
    }
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPFlowRemovedFlags> {
  static void bitset(IO &io, ofp::OFPFlowRemovedFlags &value) {
    OFP_YAML_BITCASE(OFPRRF_, IDLE_TIMEOUT);
    OFP_YAML_BITCASE(OFPRRF_, HARD_TIMEOUT);
    OFP_YAML_BITCASE(OFPRRF_, DELETE);
    OFP_YAML_BITCASE(OFPRRF_, GROUP_DELETE);
    OFP_YAML_BITCASE(OFPRRF_, METER_DELETE);
    OFP_YAML_BITCASE(OFPRRF_, EVICTION);

    io.bitSetCaseOther(value, ofp::OFPRRF_OTHER_FLOWREMOVED_FLAGS);

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(io, val,
                              ofp::yaml::AllFlags<ofp::OFPFlowRemovedFlags>());
    }
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPTableConfigFlags> {
  static void bitset(IO &io, ofp::OFPTableConfigFlags &value) {
    OFP_YAML_BITCASE(OFPTC_, EVICTION);
    OFP_YAML_BITCASE(OFPTC_, VACANCY_EVENTS);

    if (ofp::yaml::GetVersionFromContext(io) <= ofp::OFP_VERSION_3) {
      if ((value & ofp::OFPTC_TABLE_MISS_MASK) == ofp::OFPTC_TABLE_MISS_MASK) {
        // Value 0x03 is unused. Make sure it is displayed correctly.
        io.bitSetCaseOther(value, ofp::OFPTC_OTHER_TABLE_CONFIG_FLAGS_ALL);
      } else {
        OFP_YAML_MASKEDBITCASE(OFPTC_, TABLE_MISS_CONTROLLER,
                               OFPTC_TABLE_MISS_MASK);
        OFP_YAML_MASKEDBITCASE(OFPTC_, TABLE_MISS_CONTINUE,
                               OFPTC_TABLE_MISS_MASK);
        OFP_YAML_MASKEDBITCASE(OFPTC_, TABLE_MISS_DROP, OFPTC_TABLE_MISS_MASK);
        io.bitSetCaseOther(value, ofp::OFPTC_OTHER_TABLE_CONFIG_FLAGS_V2);
      }
    } else {
      io.bitSetCaseOther(value, ofp::OFPTC_OTHER_TABLE_CONFIG_FLAGS);
    }

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(io, val,
                              ofp::yaml::AllFlags<ofp::OFPTableConfigFlags>());
    }
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPRoleStatusFlags> {
  static void bitset(IO &io, ofp::OFPRoleStatusFlags &value) {
    OFP_YAML_BITCASE(OFPCRRF_, MASTER_REQUEST);
    OFP_YAML_BITCASE(OFPCRRF_, CONFIG);
    OFP_YAML_BITCASE(OFPCRRF_, EXPERIMENTER);

    io.bitSetCaseOther(value, ofp::OFPCRRF_OTHER_ROLESTATUS_FLAGS);

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(io, val,
                              ofp::yaml::AllFlags<ofp::OFPRoleStatusFlags>());
    }
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPTableStatusFlags> {
  static void bitset(IO &io, ofp::OFPTableStatusFlags &value) {
    OFP_YAML_BITCASE(OFPTRF_, VACANCY_DOWN);
    OFP_YAML_BITCASE(OFPTRF_, VACANCY_UP);

    io.bitSetCaseOther(value, ofp::OFPTRF_OTHER_TABLESTATUS_FLAGS);

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(io, val,
                              ofp::yaml::AllFlags<ofp::OFPTableStatusFlags>());
    }
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPRequestForwardFlags> {
  static void bitset(IO &io, ofp::OFPRequestForwardFlags &value) {
    OFP_YAML_BITCASE(OFPRFRF_, GROUP_MOD);
    OFP_YAML_BITCASE(OFPRFRF_, METER_MOD);

    io.bitSetCaseOther(value, ofp::OFPRFRF_OTHER_REQUESTFORWARD_FLAGS);

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(
          io, val, ofp::yaml::AllFlags<ofp::OFPRequestForwardFlags>());
    }
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPFlowMonitorFlags> {
  static void bitset(IO &io, ofp::OFPFlowMonitorFlags &value) {
    OFP_YAML_BITCASE(OFPFMF_, INITIAL);
    OFP_YAML_BITCASE(OFPFMF_, ADD);
    OFP_YAML_BITCASE(OFPFMF_, REMOVED);
    OFP_YAML_BITCASE(OFPFMF_, MODIFY);
    OFP_YAML_BITCASE(OFPFMF_, INSTRUCTIONS);
    OFP_YAML_BITCASE(OFPFMF_, NO_ABBREV);
    OFP_YAML_BITCASE(OFPFMF_, ONLY_OWN);

    io.bitSetCaseOther(value, ofp::OFPFMF_OTHER_FLOW_MONITOR_FLAGS);

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(io, val,
                              ofp::yaml::AllFlags<ofp::OFPFlowMonitorFlags>());
    }
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPBundleFlags> {
  static void bitset(IO &io, ofp::OFPBundleFlags &value) {
    OFP_YAML_BITCASE(OFPBF_, ATOMIC);
    OFP_YAML_BITCASE(OFPBF_, ORDERED);

    io.bitSetCaseOther(value, ofp::OFPBF_OTHER_BUNDLE_FLAGS);

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(io, val,
                              ofp::yaml::AllFlags<ofp::OFPBundleFlags>());
    }
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPMeterBandFlags> {
  static void bitset(IO &io, ofp::OFPMeterBandFlags &value) {
    OFP_YAML_BITCASE(OFPMBTF_, NONE);
    OFP_YAML_BITCASE(OFPMBTF_, DROP);
    OFP_YAML_BITCASE(OFPMBTF_, DSCP_REMARK);

    io.bitSetCaseOther(value, ofp::OFPMBTF_OTHER_METERBAND_FLAGS);

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(io, val,
                              ofp::yaml::AllFlags<ofp::OFPMeterBandFlags>());
    }
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPGroupTypeFlags> {
  static void bitset(IO &io, ofp::OFPGroupTypeFlags &value) {
    OFP_YAML_BITCASE(OFPGTF_, ALL);
    OFP_YAML_BITCASE(OFPGTF_, SELECT);
    OFP_YAML_BITCASE(OFPGTF_, INDIRECT);
    OFP_YAML_BITCASE(OFPGTF_, FF);

    io.bitSetCaseOther(value, ofp::OFPGTF_OTHER_GROUP_FLAGS);

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(io, val,
                              ofp::yaml::AllFlags<ofp::OFPGroupTypeFlags>());
    }
  }
};

template <>
struct ScalarBitSetTraits<ofp::OFPGroupCapabilityFlags> {
  static void bitset(IO &io, ofp::OFPGroupCapabilityFlags &value) {
    OFP_YAML_BITCASE(OFPGFC_, SELECT_WEIGHT);
    OFP_YAML_BITCASE(OFPGFC_, SELECT_LIVENESS);
    OFP_YAML_BITCASE(OFPGFC_, CHAINING);
    OFP_YAML_BITCASE(OFPGFC_, CHAINING_CHECKS);

    io.bitSetCaseOther(value, ofp::OFPGFC_OTHER_GROUP_FLAGS);

    auto val = io.bitSetCaseUnmatched();
    if (!val.empty()) {
      ofp::yaml::SetFlagError(
          io, val, ofp::yaml::AllFlags<ofp::OFPGroupCapabilityFlags>());
    }
  }
};

#undef OFP_YAML_BITCASE
#undef OFP_YAML_BITCASE_V1
#undef OFP_YAML_MASKEDBITCASE
#undef OFP_ENUM_CONVERTER

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YCONSTANTS_H_
