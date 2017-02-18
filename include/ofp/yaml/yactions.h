// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YACTIONS_H_
#define OFP_YAML_YACTIONS_H_

#include "ofp/actionfulltype.h"
#include "ofp/actionlist.h"
#include "ofp/actions.h"
#include "ofp/nicira.h"
#include "ofp/yaml/yactionfulltype.h"
#include "ofp/yaml/ycontrollermaxlen.h"
#include "ofp/yaml/ymatch.h"
#include "ofp/yaml/yoxmregister.h"
#include "ofp/yaml/yqueuenumber.h"

namespace ofp {
namespace detail {

struct ActionInserter {};

OFP_BEGIN_IGNORE_PADDING

class SetFieldInserter {
 public:
  SetFieldInserter(llvm::yaml::IO &io, ActionList &list, OXMFullType type)
      : io_(io), list_(list), type_{type} {
    assert(!type_.hasMask());
  }

  template <class ValueType>
  void visit() {
    typename ValueType::NativeType value;
    io_.mapRequired("value", value);
    AT_SET_FIELD<ValueType> action{ValueType{value}};
    list_.add(action);
  }

 private:
  llvm::yaml::IO &io_;
  ActionList &list_;
  OXMFullType type_;
};

OFP_END_IGNORE_PADDING

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

const char *const kCopyTTLOutSchema = R"""({Action/COPY_TTL_OUT}
action: COPY_TTL_OUT
)""";

const char *const kCopyTTLInSchema = R"""({Action/COPY_TTL_IN}
action: COPY_TTL_IN
)""";

const char *const kDecMPLSTTLSchema = R"""({Action/DEC_MPLS_TTL}
action: DEC_MPLS_TTL
)""";

const char *const kPopVLANSchema = R"""({Action/POP_VLAN}
action: POP_VLAN
)""";

const char *const kDecNwTTLSchema = R"""({Action/DEC_NW_TTL}
action: DEC_NW_TTL
)""";

const char *const kPopPBBSchema = R"""({Action/POP_PBB}
action: POP_PBB
)""";

const char *const kOutputSchema = R"""({Action/OUTPUT}
action: OUTPUT
port_no: PortNumber
max_len: ControllerMaxLen
)""";

const char *const kSetMPLSTTLSchema = R"""({Action/SET_MPLS_TTL}
action: SET_MPLS_TTL
ttl: UInt16
)""";

const char *const kPushVLANSchema = R"""({Action/PUSH_VLAN}
action: PUSH_VLAN
ethertype: UInt16
)""";

const char *const kPushMPLSSchema = R"""({Action/PUSH_MPLS}
action: PUSH_MPLS
ethertype: UInt16
)""";

const char *const kPopMPLSSchema = R"""({Action/POP_MPLS}
action: POP_MPLS
ethertype: UInt16
)""";

const char *const kSetQueueSchema = R"""({Action/SET_QUEUE}
action: SET_QUEUE
queue_id: QueueNumber
)""";

const char *const kGroupSchema = R"""({Action/GROUP}
action: GROUP
group_id: GroupNumber
)""";

const char *const kSetNwTTLSchema = R"""({Action/SET_NW_TTL}
action: SET_NW_TTL
ttl: UInt8
)""";

const char *const kPushPBBSchema = R"""({Action/PUSH_PBB}
action: PUSH_PBB
ethertype: UInt16
)""";

const char *const kEnqueueSchema = R"""({Action/ENQUEUE}
action: ENQUEUE
port_no: PortNumber
queue_id: QueueNumber
)""";

const char *const kSetFieldSchema = R"""({Action/SET_FIELD}
action: SET_FIELD
field: <Field-Name>
value: <Field-Value>
)""";

const char *const kExperimenterActionSchema = R"""({Action/EXPERIMENTER}
action: EXPERIMENTER
experimenter: UInt32
data: HexData
)""";

const char *const kNiciraRegMoveActionSchema = R"""({Action/NX_REG_MOVE}
action: NX_REG_MOVE
src: RegisterBits
dst: RegisterBits
)""";

const char *const kNiciraRegLoadActionSchema = R"""({Action/NX_REG_LOAD}
action: NX_REG_MOVE
dst: RegisterBits
value: UInt64
)""";

template <>
struct MappingTraits<ofp::detail::ActionIteratorItem> {
  static void mapping(IO &io, ofp::detail::ActionIteratorItem &item) {
    using namespace ofp;

    ActionFullType fullType{item.type(), item.experimenter(), item.subtype()};
    io.mapRequired("action", fullType);

    switch (fullType.type()) {
      case AT_COPY_TTL_OUT::type():
      case AT_COPY_TTL_IN::type():
      case AT_DEC_MPLS_TTL::type():
      case AT_POP_VLAN::type():
      case AT_DEC_NW_TTL::type():
      case AT_POP_PBB::type():
        // Nothing else to do.
        break;
      case AT_OUTPUT::type(): {
        const AT_OUTPUT *action = item.action<AT_OUTPUT>();
        PortNumber port = action->port();
        ControllerMaxLen maxlen = action->maxlen();
        io.mapRequired("port_no", port);
        io.mapRequired("max_len", maxlen);
        break;
      }
      case AT_SET_MPLS_TTL::type(): {
        const AT_SET_MPLS_TTL *action = item.action<AT_SET_MPLS_TTL>();
        Hex16 ttl = action->ttl();
        io.mapRequired("ttl", ttl);
        break;
      }
      case AT_PUSH_VLAN::type(): {
        const AT_PUSH_VLAN *action = item.action<AT_PUSH_VLAN>();
        Hex16 ethertype = action->ethertype();
        io.mapRequired("ethertype", ethertype);
        break;
      }
      case AT_PUSH_MPLS::type(): {
        const AT_PUSH_MPLS *action = item.action<AT_PUSH_MPLS>();
        Hex16 ethertype = action->ethertype();
        io.mapRequired("ethertype", ethertype);
        break;
      }
      case AT_POP_MPLS::type(): {
        const AT_POP_MPLS *action = item.action<AT_POP_MPLS>();
        Hex16 ethertype = action->ethertype();
        io.mapRequired("ethertype", ethertype);
        break;
      }
      case AT_SET_QUEUE::type(): {
        const AT_SET_QUEUE *action = item.action<AT_SET_QUEUE>();
        QueueNumber queue = action->queue();
        io.mapRequired("queue_id", queue);
        break;
      }
      case AT_GROUP::type(): {
        const AT_GROUP *action = item.action<AT_GROUP>();
        GroupNumber group = action->group();
        io.mapRequired("group_id", group);
        break;
      }
      case AT_SET_NW_TTL::type(): {
        const AT_SET_NW_TTL *action = item.action<AT_SET_NW_TTL>();
        Hex8 ttl = action->ttl();
        io.mapRequired("ttl", ttl);
        break;
      }
      case AT_PUSH_PBB::type(): {
        const AT_PUSH_PBB *action = item.action<AT_PUSH_PBB>();
        Hex16 ethertype = action->ethertype();
        io.mapRequired("ethertype", ethertype);
        break;
      }
      case deprecated::AT_ENQUEUE_V1::type(): {
        const deprecated::AT_ENQUEUE_V1 *action =
            item.action<deprecated::AT_ENQUEUE_V1>();
        PortNumber port = action->port();
        Hex32 queueId = action->queueId();
        io.mapRequired("port_no", port);
        io.mapRequired("queue_id", queueId);
        break;
      }
      default: {
        // Variable length actions, known actions with the unexpected lengths,
        // and unknown actions.
        switch (fullType.enumType()) {
          case OFPAT_EXPERIMENTER: {
            const AT_EXPERIMENTER *action = item.action<AT_EXPERIMENTER>();
            if (fullType.experimenter() == nx::NICIRA) {
              handleNicira(io, action);
            } else {
              ByteRange value = action->value();
              io.mapRequired("data", value);
            }
            break;
          }
          case OFPAT_SET_FIELD: {
            OXMIterator iter = item.oxmRange().begin();
            OXMFullType oxmType{iter->type(), iter->experimenter()};
            io.mapRequired("field", oxmType);
            OXMInternalID id = oxmType.internalID();
            if (id != ofp::OXMInternalID::UNKNOWN) {
              ofp::detail::OXMItemReader reader{io, RemoveConst_cast(*iter),
                                                oxmType};
              OXMDispatch(id, &reader);
            } else {
              ByteRange data{iter->unknownValuePtr(),
                             iter->unknownValueLength()};
              io.mapRequired("value", data);
            }
            break;
          }
          default:
            ByteRange data = item.value();
            io.mapRequired("data", data);
            break;
        }
        break;
      }
    }
  }

 private:
  static void handleNicira(IO &io, const ofp::AT_EXPERIMENTER *action) {
    using namespace ofp;
    switch (action->subtype()) {
      case nx::AT_REGMOVE::subtype(): {
        const nx::AT_REGMOVE *regmove = nx::AT_REGMOVE::cast(action);
        OXMRegister src = regmove->src();
        OXMRegister dst = regmove->dst();
        io.mapRequired("src", src);
        io.mapRequired("dst", dst);
        break;
      }
      case nx::AT_REGLOAD::subtype(): {
        const nx::AT_REGLOAD *regload = nx::AT_REGLOAD::cast(action);
        OXMRegister dst = regload->dst();
        Big64 value = regload->value();
        io.mapRequired("dst", dst);
        io.mapRequired("value", value);
        break;
      }
      default: {
        ByteRange value = action->value();
        io.mapRequired("data", value);
      }
    }
  }
};

template <>
struct MappingTraits<ofp::detail::ActionInserter> {
  static void mapping(IO &io, ofp::detail::ActionInserter &builder) {
    using namespace ofp;

    ActionList &list = Ref_cast<ActionList>(builder);

    ActionFullType fullType;
    io.mapRequired("action", fullType);

    if (fullType.type() == deprecated::AT_ENQUEUE_V1::type()) {
      PortNumber port;
      Hex32 queueId;
      io.mapRequired("port_no", port);
      io.mapRequired("queue_id", queueId);
      deprecated::AT_ENQUEUE_V1 action{port, queueId};
      list.add(action);
      return;
    }

    switch (fullType.enumType()) {
      case OFPAT_COPY_TTL_OUT: {
        AT_COPY_TTL_OUT action;
        list.add(action);
        break;
      }
      case OFPAT_COPY_TTL_IN: {
        AT_COPY_TTL_IN action;
        list.add(action);
        break;
      }
      case OFPAT_DEC_MPLS_TTL: {
        AT_DEC_MPLS_TTL action;
        list.add(action);
        break;
      }
      case OFPAT_POP_VLAN: {
        AT_POP_VLAN action;
        list.add(action);
        break;
      }
      case OFPAT_DEC_NW_TTL: {
        AT_DEC_NW_TTL action;
        list.add(action);
        break;
      }
      case OFPAT_POP_PBB: {
        AT_POP_PBB action;
        list.add(action);
        break;
      }
      case OFPAT_OUTPUT: {
        PortNumber port;
        ControllerMaxLen maxlen;
        io.mapRequired("port_no", port);
        io.mapRequired("max_len", maxlen);
        AT_OUTPUT action{port, maxlen};
        list.add(action);
        break;
      }
      case OFPAT_SET_MPLS_TTL: {
        UInt16 ttl;
        io.mapRequired("ttl", ttl);
        AT_SET_MPLS_TTL action{ttl};
        list.add(action);
        break;
      }
      case OFPAT_PUSH_VLAN: {
        UInt16 ethertype;
        io.mapRequired("ethertype", ethertype);
        AT_PUSH_VLAN action{ethertype};
        list.add(action);
        break;
      }
      case OFPAT_PUSH_MPLS: {
        UInt16 ethertype;
        io.mapRequired("ethertype", ethertype);
        AT_PUSH_MPLS action{ethertype};
        list.add(action);
        break;
      }
      case OFPAT_POP_MPLS: {
        UInt16 ethertype;
        io.mapRequired("ethertype", ethertype);
        AT_POP_MPLS action{ethertype};
        list.add(action);
        break;
      }
      case OFPAT_SET_QUEUE: {
        UInt32 queue;
        io.mapRequired("queue_id", queue);
        AT_SET_QUEUE action{queue};
        list.add(action);
        break;
      }
      case OFPAT_GROUP: {
        GroupNumber group;
        io.mapRequired("group_id", group);
        AT_GROUP action{group};
        list.add(action);
        break;
      }
      case OFPAT_SET_NW_TTL: {
        UInt8 ttl;
        io.mapRequired("ttl", ttl);
        AT_SET_NW_TTL action{ttl};
        list.add(action);
        break;
      }
      case OFPAT_PUSH_PBB: {
        UInt16 ethertype;
        io.mapRequired("ethertype", ethertype);
        AT_PUSH_PBB action{ethertype};
        list.add(action);
        break;
      }
      case OFPAT_EXPERIMENTER: {
        if (fullType.experimenter() == nx::NICIRA) {
          addNicira(io, list, fullType);
        } else {
          UInt32 experimenterid;
          ByteList value;
          if (fullType.experimenter() == 0) {
            io.mapRequired("experimenter", experimenterid);
          } else {
            experimenterid = fullType.experimenter();
          }
          io.mapRequired("data", value);
          AT_EXPERIMENTER action{experimenterid, value};
          list.add(action);
        }
        break;
      }
      case OFPAT_SET_FIELD: {
        OXMFullType oxmType;
        io.mapRequired("field", oxmType);
        ofp::OXMInternalID id = oxmType.internalID();
        if (id != ofp::OXMInternalID::UNKNOWN) {
          ofp::detail::SetFieldInserter inserter{io, list, oxmType};
          OXMDispatch(id, &inserter);
        } else {
          ByteList data;
          io.mapRequired("value", data);
          // FIXME(bfish): Handle experimenter oxm's.
          AT_SET_FIELD_CUSTOM action{oxmType.type(), oxmType.experimenter(),
                                     data};
          list.add(action);
        }
        break;
      }
      default:
        ByteList data;
        io.mapRequired("data", data);
        AT_UNKNOWN action{fullType.type(), data};
        list.add(action);
        break;
    }
  }

 private:
  static void addNicira(IO &io, ofp::ActionList &list,
                        const ofp::ActionFullType &fullType) {
    using namespace ofp;

    switch (fullType.subtype()) {
      case nx::AT_REGMOVE::subtype(): {
        OXMRegister src;
        OXMRegister dst;
        io.mapRequired("src", src);
        io.mapRequired("dst", dst);
        nx::AT_REGMOVE action{src, dst};
        list.add(action);
        break;
      }
      case nx::AT_REGLOAD::subtype(): {
        OXMRegister dst;
        Big64 value;
        io.mapRequired("dst", dst);
        io.mapRequired("value", value);
        nx::AT_REGLOAD action{value, dst};
        list.add(action);
        break;
      }
      default: {
        ByteList value;
        io.mapRequired("data", value);
        AT_EXPERIMENTER action{nx::NICIRA, value};
        list.add(action);
      }
    }
  }
};

template <>
struct SequenceTraits<ofp::ActionRange> {
  using iterator = ofp::ActionIterator;

  static iterator begin(IO &io, ofp::ActionRange &range) {
    return range.begin();
  }

  static iterator end(IO &io, ofp::ActionRange &range) { return range.end(); }

  static void next(iterator &iter, iterator iterEnd) { ++iter; }
};

template <>
struct SequenceTraits<ofp::ActionList> {
  static size_t size(IO &io, ofp::ActionList &list) { return 0; }

  static ofp::detail::ActionInserter &element(IO &io, ofp::ActionList &list,
                                              size_t index) {
    return Ref_cast<ofp::detail::ActionInserter>(list);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YACTIONS_H_
