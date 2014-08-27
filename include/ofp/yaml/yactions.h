//  ===== ---- ofp/yaml/yactions.h -------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the llvm::yaml::MappingTraits for all actions, ActionRange,
/// and ActionList.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YACTIONS_H_
#define OFP_YAML_YACTIONS_H_

#include "ofp/actionlist.h"
#include "ofp/actions.h"
#include "ofp/yaml/ymatch.h"
#include "ofp/yaml/yactiontype.h"

namespace ofp {
namespace detail {

struct ActionInserter {};

OFP_BEGIN_IGNORE_PADDING

class SetFieldInserter {
public:
    SetFieldInserter(llvm::yaml::IO &io, ActionList &list, OXMType type)
        : io_(io), list_(list), type_{type}
    {
    	assert(!type_.hasMask());
    }

    template <class ValueType>
    void visit()
    {
        typename ValueType::NativeType value;
        io_.mapRequired("value", value);
        AT_SET_FIELD<ValueType> action{ValueType{value}};
        list_.add(action);
    }

private:
    llvm::yaml::IO &io_;
    ActionList &list_;
    OXMType type_;
};

OFP_END_IGNORE_PADDING

}  // namespace detail
}  // namespace ofp


namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::detail::ActionIteratorItem> {

    static void mapping(IO &io, ofp::detail::ActionIteratorItem &item)
    {
    	using namespace ofp;

    	ActionType type = item.type();
    	io.mapRequired("action", type);

    	switch (type)
    	{
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
				Hex32 port = action->port();
				Hex16 maxlen = action->maxlen();
				io.mapRequired("port", port);
				io.mapRequired("maxlen", maxlen);
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
				Hex16 vlan = action->vlan();
				io.mapRequired("vlan", vlan);
				break;
			}
			case AT_PUSH_MPLS::type(): {
				const AT_PUSH_MPLS *action = item.action<AT_PUSH_MPLS>();
				Hex32 mpls = action->mpls();
				io.mapRequired("mpls", mpls);
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
				Hex32 queue = action->queue();
				io.mapRequired("queue", queue);
				break;
			}
			case AT_GROUP::type(): {
				const AT_GROUP *action = item.action<AT_GROUP>();
				Hex32 group = action->group();
				io.mapRequired("group", group);
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
            default: {
                switch (type.enumType()) {
                case OFPAT_EXPERIMENTER: {
                    const AT_EXPERIMENTER *action = item.action<AT_EXPERIMENTER>();
                    Hex32 experimenterid = action->experimenterid();
                    io.mapRequired("experimenter", experimenterid);
                    ByteRange value = action->value();
                    io.mapRequired("data", value);
                    break;
                }
                case OFPAT_SET_FIELD: {
                    OXMIterator iter = item.oxmRange().begin();
                    OXMType oxmType = iter.type();
                    io.mapRequired("field", oxmType);
                    OXMInternalID id = oxmType.internalID();
                    if (id != ofp::OXMInternalID::UNKNOWN) {
                      ofp::detail::OXMItemReader reader{io, RemoveConst_cast(*iter), oxmType};
                      OXMDispatch(id, &reader);
                    } else {
                      ByteRange data{iter->unknownValuePtr(), oxmType.length()};
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
};

template <>
struct MappingTraits<ofp::detail::ActionInserter> {

    static void mapping(IO &io, ofp::detail::ActionInserter &builder)
    {
    	using namespace ofp;

    	ActionList &list = Ref_cast<ActionList>(builder);

    	ActionType type;
    	io.mapRequired("action", type);
    	switch (type.enumType())
    	{
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
				UInt32 port;
				UInt16 maxlen;
				io.mapRequired("port", port);
				io.mapRequired("maxlen", maxlen);
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
				UInt16 vlan;
				io.mapRequired("vlan", vlan);
				AT_PUSH_VLAN action{vlan};
				list.add(action);
				break;
			}
			case OFPAT_PUSH_MPLS: {
				UInt32 mpls;
				io.mapRequired("mpls", mpls);
				AT_PUSH_MPLS action{mpls};
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
				io.mapRequired("queue", queue);
				AT_SET_QUEUE action{queue};
				list.add(action);
				break;
			}
			case OFPAT_GROUP: {
				UInt32 group;
				io.mapRequired("group", group);
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
				UInt32 experimenterid;
				io.mapRequired("experimenter", experimenterid);
                ByteList value;
                io.mapRequired("data", value);
				AT_EXPERIMENTER action{experimenterid, value};
				list.add(action);
				break;
			}
			case OFPAT_SET_FIELD: {
				OXMType oxmType;
				io.mapRequired("field", oxmType);
				ofp::OXMInternalID id = oxmType.internalID();
        		if (id != ofp::OXMInternalID::UNKNOWN) {
            		ofp::detail::SetFieldInserter inserter{io, list, oxmType};
            		OXMDispatch(id, &inserter);
        		} else {
                    ByteList data;
                    io.mapRequired("value", data);
                    AT_SET_FIELD_CUSTOM action{oxmType, data};
                    list.add(action);
                }
				break;
			}
			default:
                ByteList data;
                io.mapRequired("data", data);
                AT_UNKNOWN action{type, data};
                list.add(action);
				//log::info("MappingTraits<Inserter>: Unknown Action type:", int(type));
				break;
    	}
    }
};


template <>
struct SequenceTraits<ofp::ActionRange> {
    using iterator = ofp::ActionIterator;

    static iterator begin(IO &io, ofp::ActionRange &range) {
        return range.begin();
    }

    static iterator end(IO &io, ofp::ActionRange &range) {
        return range.end();
    }

    static void next(iterator &iter, iterator iterEnd) {
        ++iter;
    }
};


template <>
struct SequenceTraits<ofp::ActionList> {

    static size_t size(IO &io, ofp::ActionList &list)
    {
        return 0;
    }

    static ofp::detail::ActionInserter &element(IO &io, ofp::ActionList &list,
                                     size_t index)
    {
        return Ref_cast<ofp::detail::ActionInserter>(list);
    }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YACTIONS_H_
