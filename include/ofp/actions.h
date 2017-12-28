// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_ACTIONS_H_
#define OFP_ACTIONS_H_

#include "ofp/actiontype.h"
#include "ofp/constants.h"
#include "ofp/controllermaxlen.h"
#include "ofp/groupnumber.h"
#include "ofp/oxmfields.h"
#include "ofp/oxmtype.h"
#include "ofp/padding.h"
#include "ofp/portnumber.h"
#include "ofp/queuenumber.h"
#include "ofp/validation.h"

namespace ofp {

class Validation;

namespace detail {

template <OFPActionType ActType>
class EmptyAction {
 public:
  constexpr static ActionType type() { return ActionType(ActType, 8); }

  constexpr EmptyAction() {}

 private:
  const ActionType type_ = type();
  const Padding<4> pad_;
};

}  // namespace detail

/// \brief Concrete types for various empty actions.

using AT_COPY_TTL_OUT = detail::EmptyAction<OFPAT_COPY_TTL_OUT>;
using AT_COPY_TTL_IN = detail::EmptyAction<OFPAT_COPY_TTL_IN>;
using AT_DEC_MPLS_TTL = detail::EmptyAction<OFPAT_DEC_MPLS_TTL>;
using AT_POP_VLAN = detail::EmptyAction<OFPAT_POP_VLAN>;
using AT_DEC_NW_TTL = detail::EmptyAction<OFPAT_DEC_NW_TTL>;
using AT_POP_PBB = detail::EmptyAction<OFPAT_POP_PBB>;

static_assert(sizeof(AT_COPY_TTL_OUT) == 8, "Unexpected size.");
static_assert(IsStandardLayout<AT_COPY_TTL_OUT>(), "Unexpected layout.");

/// \brief Concrete type for AT_OUTPUT action.
class AT_OUTPUT {
 public:
  constexpr static ActionType type() { return ActionType(OFPAT_OUTPUT, 16); }

  constexpr explicit AT_OUTPUT(PortNumber port, ControllerMaxLen maxlen = 0)
      : port_{port}, maxlen_{maxlen} {}

  constexpr PortNumber port() const { return port_; }
  constexpr ControllerMaxLen maxlen() const { return maxlen_; }

 private:
  const ActionType type_ = type();
  const PortNumber port_;
  const ControllerMaxLen maxlen_;
  Padding<6> pad_;
};

static_assert(sizeof(AT_OUTPUT) == 16, "Unexpected size.");
static_assert(IsStandardLayout<AT_OUTPUT>(), "Unexpected layout");

namespace deprecated {

class AT_OUTPUT_V1 {
 public:
  constexpr static ActionType type() { return ActionType(OFPAT_OUTPUT, 8); }

  constexpr explicit AT_OUTPUT_V1(const AT_OUTPUT *output)
      : port_{UInt16_narrow_cast(output->port())}, maxlen_{output->maxlen()} {}

  constexpr UInt16 port() const { return port_; }
  constexpr UInt16 maxlen() const { return maxlen_; }

 private:
  const ActionType type_ = type();
  const Big16 port_;
  const Big16 maxlen_;
};

static_assert(sizeof(AT_OUTPUT_V1) == 8, "Unexpected size.");
static_assert(IsStandardLayout<AT_OUTPUT_V1>(), "Unexpected layout");

}  // namespace deprecated

/// \brief Concrete type for AT_SET_MPLS_TTL action.
class AT_SET_MPLS_TTL {
 public:
  constexpr static ActionType type() {
    return ActionType(OFPAT_SET_MPLS_TTL, 8);
  }

  constexpr explicit AT_SET_MPLS_TTL(UInt16 ttl) : ttl_{ttl} {}

  UInt16 ttl() const { return ttl_; }

 private:
  const ActionType type_ = type();
  const Big16 ttl_;
  const Padding<2> pad_;
};

static_assert(sizeof(AT_SET_MPLS_TTL) == 8, "Unexpected size.");
static_assert(IsStandardLayout<AT_SET_MPLS_TTL>(), "Unexpected layout");

/// \brief Concrete type for AT_PUSH_VLAN action.
class AT_PUSH_VLAN {
 public:
  constexpr static ActionType type() { return ActionType(OFPAT_PUSH_VLAN, 8); }

  constexpr explicit AT_PUSH_VLAN(UInt16 ethertype) : ethertype_{ethertype} {}

  UInt16 ethertype() const { return ethertype_; }

 private:
  const ActionType type_ = type();
  const Big16 ethertype_;
  const Padding<2> pad_;
};

static_assert(sizeof(AT_PUSH_VLAN) == 8, "Unexpected size.");
static_assert(IsStandardLayout<AT_PUSH_VLAN>(), "Unexpected layout");

/// \brief Concrete type for AT_PUSH_MPLS action.
class AT_PUSH_MPLS {
 public:
  constexpr static ActionType type() { return ActionType(OFPAT_PUSH_MPLS, 8); }

  constexpr explicit AT_PUSH_MPLS(UInt16 ethertype) : ethertype_{ethertype} {}

  UInt16 ethertype() const { return ethertype_; }

 private:
  const ActionType type_ = type();
  const Big16 ethertype_;
  const Padding<2> pad_;
};

static_assert(sizeof(AT_PUSH_MPLS) == 8, "Unexpected size.");
static_assert(IsStandardLayout<AT_PUSH_MPLS>(), "Unexpected layout");

/// \brief Concrete type for AT_POP_MPLS action.
class AT_POP_MPLS {
 public:
  constexpr static ActionType type() { return ActionType(OFPAT_POP_MPLS, 8); }

  constexpr explicit AT_POP_MPLS(UInt16 ethertype) : ethertype_{ethertype} {}

  UInt16 ethertype() const { return ethertype_; }

 private:
  const ActionType type_ = type();
  const Big16 ethertype_;
  const Padding<2> pad_;
};

static_assert(sizeof(AT_PUSH_MPLS) == 8, "Unexpected size.");
static_assert(IsStandardLayout<AT_PUSH_MPLS>(), "Unexpected layout");

/// \brief Concrete type for AT_SET_QUEUE action.
class AT_SET_QUEUE {
 public:
  constexpr static ActionType type() { return ActionType(OFPAT_SET_QUEUE, 8); }

  constexpr explicit AT_SET_QUEUE(QueueNumber queue) : queue_{queue} {}

  QueueNumber queue() const { return queue_; }

 private:
  const ActionType type_ = type();
  const QueueNumber queue_;
};

static_assert(sizeof(AT_SET_QUEUE) == 8, "Unexpected size.");
static_assert(IsStandardLayout<AT_SET_QUEUE>(), "Unexpected layout");

/// \brief Concrete type for AT_GROUP action.
class AT_GROUP {
 public:
  constexpr static ActionType type() { return ActionType(OFPAT_GROUP, 8); }

  constexpr explicit AT_GROUP(GroupNumber group) : group_{group} {}

  GroupNumber group() const { return group_; }

 private:
  const ActionType type_ = type();
  const GroupNumber group_;
};

static_assert(sizeof(AT_GROUP) == 8, "Unexpected size.");
static_assert(IsStandardLayout<AT_GROUP>(), "Unexpected layout");

/// \brief Concrete type for AT_SET_NW_TTL action.
class AT_SET_NW_TTL {
 public:
  constexpr static ActionType type() { return ActionType(OFPAT_SET_NW_TTL, 8); }

  constexpr explicit AT_SET_NW_TTL(UInt8 ttl) : ttl_{ttl} {}

  UInt8 ttl() const { return ttl_; }

 private:
  const ActionType type_ = type();
  const Big8 ttl_;
  const Padding<3> pad_;
};

static_assert(sizeof(AT_SET_NW_TTL) == 8, "Unexpected size.");
static_assert(IsStandardLayout<AT_SET_NW_TTL>(), "Unexpected layout");

/// \brief Concrete type for AT_SET_NW_TTL action.
class AT_PUSH_PBB {
 public:
  constexpr static ActionType type() { return ActionType(OFPAT_PUSH_PBB, 8); }

  constexpr explicit AT_PUSH_PBB(UInt16 ethertype) : ethertype_{ethertype} {}

  UInt16 ethertype() const { return ethertype_; }

 private:
  const ActionType type_ = type();
  const Big16 ethertype_;
  const Padding<2> pad_;
};

static_assert(sizeof(AT_PUSH_PBB) == 8, "Unexpected size.");
static_assert(IsStandardLayout<AT_PUSH_PBB>(), "Unexpected layout");

/// \brief Concrete type for AT_EXPERIMENTER action.
class AT_EXPERIMENTER {
 public:
  // Variable length actions do not have a type().

  constexpr explicit AT_EXPERIMENTER(UInt32 experimenterid,
                                     const ByteRange &value)
      : type_{OFPAT_EXPERIMENTER,
              UInt16_narrow_cast(PadLength(FixedSize + value.size()))},
        experimenterid_{experimenterid},
        value_{value} {}

  UInt32 experimenterid() const { return experimenterid_; }
  UInt16 size() const { return type_.length(); }
  ByteRange value() const {
    return SafeByteRange(this, type_.length(), FixedSize);
  }

  bool validateInput(Validation *context) const {
    return context->validateBool(type_.length() >= FixedSize,
                                 "Invalid EXPERIMENTER action");
  }

  enum : size_t { MinSizeWithSubtype = 10 };

  UInt16 subtype() const { return *Big16_cast(BytePtr(this) + FixedSize); }

 private:
  const ActionType type_;
  const Big32 experimenterid_;
  const ByteRange value_;

  enum : size_t { FixedSize = 8 };

  friend class ActionList;
};

/// \brief Concrete type for AT_SET_FIELD action.
template <class ValueType>
class AT_SET_FIELD {
 public:
  constexpr static ActionType type() {
    return ActionType(OFPAT_SET_FIELD, 8 + sizeof(Padded<ValueType>));
  }

  constexpr explicit AT_SET_FIELD(const ValueType &value) : value_{value} {}

  ValueType value() const { return value_; }

 private:
  const ActionType type_ = type();
  const OXMType oxmtype_ = ValueType::type();
  const Padded<ValueType> value_;
};

OFP_BEGIN_IGNORE_PADDING

class AT_SET_FIELD_CUSTOM {
 public:
  // Variable length actions do not have a type().

  constexpr AT_SET_FIELD_CUSTOM(OXMType oxmType, Big32 experimenter,
                                const ByteRange &value)
      : type_{ActionType(OFPAT_SET_FIELD, UInt16_narrow_cast(PadLength(
                                              8U + value.size() +
                                              (experimenter != 0 ? 4 : 0))))},
        oxmtype_{oxmType},
        experimenter_{experimenter},
        value_{value} {}

  ByteRange value() const;

 private:
  const ActionType type_;
  const OXMType oxmtype_;
  const Big32 experimenter_;
  const ByteRange value_;

  enum : size_t { FixedSize = 8 };

  friend class ActionList;
};

OFP_END_IGNORE_PADDING

class AT_UNKNOWN {
 public:
  // Variable length actions do not have a type().

  constexpr AT_UNKNOWN(ActionType type, const ByteRange &value)
      : type_{ActionType(type.enumType(),
                         UInt16_narrow_cast(4U + value.size()))},
        value_{value} {}

  ByteRange value() const;

 private:
  enum : size_t { FixedSize = 4 };

  const ActionType type_;
  const Padding<4> pad_;
  const ByteRange value_;

  friend class ActionList;
};

namespace deprecated {

template <UInt16 Code, class ValueType>
class AT_SET_FIELD_V {
 public:
  constexpr static ActionType type() {
    return ActionType(static_cast<OFPActionType>(Code),
                      PadLength(4 + sizeof(ValueType)));
  }

  constexpr explicit AT_SET_FIELD_V(const AT_SET_FIELD<ValueType> *action)
      : value_{action->value()} {}

 private:
  const ActionType type_ = type();
  const ValueType value_;
  Padding<8 - (4 + sizeof(ValueType)) % 8> pad_;
};

template <UInt16 Code, class ValueType>
class AT_SET_FIELD_V_NPAD {
 public:
  constexpr static ActionType type() {
    return ActionType(static_cast<OFPActionType>(Code),
                      PadLength(4 + sizeof(ValueType)));
  }

  constexpr explicit AT_SET_FIELD_V_NPAD(const AT_SET_FIELD<ValueType> *action)
      : value_{action->value()} {}

 private:
  const ActionType type_ = type();
  const ValueType value_;
  // No padding.
};

// N.B. V1 and V2 OpenFlow's OFPAT_SET_TP_XXX actions do not make a distinction
// between type of transport protocol (TCP, UDP, SCTP), but later versions do.
// We need to map the V3 and V4 model back to V1 and V2. To do this, we
// recognize that all protocols have in common a 16-bit port number. We
// introduce an alias, OFB_TP_PORT to represent something with a 16-bit port
// number. This loss of fidelity has consequences; a client could ask that the
// source port be modified in a TCP packet using a v4 OFB_UDP_DST set-field
// action. OFP_TP_CODE serves the same purpose for ICMP and its 8-bit codes.

using OFB_TP_PORT = OFB_TCP_SRC;
using OFB_TP_CODE = OFB_ICMPV4_CODE;

// FIXME check that vid meaning is correct for extra flags.
using AT_SET_VLAN_VID_V1 = AT_SET_FIELD_V<v1::OFPAT_SET_VLAN_VID, OFB_VLAN_VID>;
using AT_SET_VLAN_PCP_V1 = AT_SET_FIELD_V<v1::OFPAT_SET_VLAN_PCP, OFB_VLAN_PCP>;
using AT_SET_DL_SRC_V1 = AT_SET_FIELD_V<v1::OFPAT_SET_DL_SRC, OFB_ETH_SRC>;
using AT_SET_DL_DST_V1 = AT_SET_FIELD_V<v1::OFPAT_SET_DL_DST, OFB_ETH_DST>;
using AT_SET_NW_SRC_V1 =
    AT_SET_FIELD_V_NPAD<v1::OFPAT_SET_NW_SRC, OFB_IPV4_SRC>;
using AT_SET_NW_DST_V1 =
    AT_SET_FIELD_V_NPAD<v1::OFPAT_SET_NW_DST, OFB_IPV4_DST>;
using AT_SET_NW_TOS_V1 = AT_SET_FIELD_V<v1::OFPAT_SET_NW_TOS, OFB_IP_DSCP>;
using AT_SET_TP_SRC_V1 = AT_SET_FIELD_V<v1::OFPAT_SET_TP_SRC, OFB_TP_PORT>;
using AT_SET_TP_DST_V1 = AT_SET_FIELD_V<v1::OFPAT_SET_TP_DST, OFB_TP_PORT>;
using AT_SET_TP_TYPE_V1 = AT_SET_FIELD_V<v1::OFPAT_SET_TP_SRC, OFB_TP_CODE>;
using AT_SET_TP_CODE_V1 = AT_SET_FIELD_V<v1::OFPAT_SET_TP_DST, OFB_TP_CODE>;

static_assert(sizeof(AT_SET_VLAN_VID_V1) == 8, "Unexpected size.");
static_assert(sizeof(AT_SET_VLAN_PCP_V1) == 8, "Unexpected size.");
static_assert(sizeof(AT_SET_DL_SRC_V1) == 16, "Unexpected size.");
static_assert(sizeof(AT_SET_DL_DST_V1) == 16, "Unexpected size.");
static_assert(sizeof(AT_SET_NW_SRC_V1) == 8, "Unexpected size.");
static_assert(sizeof(AT_SET_NW_DST_V1) == 8, "Unexpected size.");
static_assert(sizeof(AT_SET_NW_TOS_V1) == 8, "Unexpected size.");
static_assert(sizeof(AT_SET_TP_SRC_V1) == 8, "Unexpected size.");
static_assert(sizeof(AT_SET_TP_DST_V1) == 8, "Unexpected size.");
static_assert(sizeof(AT_SET_TP_TYPE_V1) == 8, "Unexpected size.");
static_assert(sizeof(AT_SET_TP_CODE_V1) == 8, "Unexpected size.");

/// \brief Concrete type for AT_ENQUEUE_v1 action.
class AT_ENQUEUE_V1 {
 public:
  constexpr static ActionType type() {
    return ActionType(static_cast<OFPActionType>(v1::OFPAT_ENQUEUE), 16);
  }

  constexpr explicit AT_ENQUEUE_V1(PortNumber port, UInt32 queueId)
      : port_{port.toV1()}, queueId_{queueId} {}

  PortNumber port() const { return PortNumber::fromV1(port_); }
  UInt32 queueId() const { return queueId_; }

 private:
  const ActionType type_ = type();
  const Big16 port_;
  const Padding<6> pad_;
  const Big32 queueId_;
};

static_assert(sizeof(AT_ENQUEUE_V1) == 16, "Unexpected size.");
static_assert(IsStandardLayout<AT_ENQUEUE_V1>(), "Unexpected layout");

}  // namespace deprecated

}  // namespace ofp

#endif  // OFP_ACTIONS_H_
