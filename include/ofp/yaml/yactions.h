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

#ifndef OFP_YAML_YACTIONS_H
#define OFP_YAML_YACTIONS_H

#include "ofp/actionlist.h"
#include "ofp/actions.h"
#include "ofp/yaml/ymatch.h"

namespace ofp { // <namespace ofp>
namespace detail { // <namespace detail>

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

} // </namespace detail>
} // </namespace ofp>


namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::ActionIterator::Item> {

    static void mapping(IO &io, ofp::ActionIterator::Item &item)
    {
    	using namespace ofp;

    	OFPActionType type = item.type().type();
    	io.mapRequired("action", type);
    	switch (type)
    	{
    		case OFPAT_COPY_TTL_OUT:
    		case OFPAT_COPY_TTL_IN:
			case OFPAT_DEC_MPLS_TTL:
			case OFPAT_POP_VLAN:
			case OFPAT_POP_MPLS:
			case OFPAT_DEC_NW_TTL:
			case OFPAT_POP_PBB:
				// Nothing else to do.
				break;
			case OFPAT_OUTPUT: {
				const AT_OUTPUT *action = item.action<AT_OUTPUT>();
				UInt32 port = action->port();
				UInt16 maxlen = action->maxlen();
				io.mapRequired("port", port);
				io.mapRequired("maxlen", maxlen);
				break;
			}
			case OFPAT_SET_MPLS_TTL: {
				const AT_SET_MPLS_TTL *action = item.action<AT_SET_MPLS_TTL>();
				UInt16 ttl = action->ttl();
				io.mapRequired("ttl", ttl);
				break;
			}
			case OFPAT_PUSH_VLAN: {
				const AT_PUSH_VLAN *action = item.action<AT_PUSH_VLAN>();
				UInt16 vlan = action->vlan();
				io.mapRequired("vlan", vlan);
				break;
			}
			case OFPAT_PUSH_MPLS: {
				const AT_PUSH_MPLS *action = item.action<AT_PUSH_MPLS>();
				UInt32 mpls = action->mpls();
				io.mapRequired("mpls", mpls);
				break;
			}
			case OFPAT_SET_QUEUE: {
				const AT_SET_QUEUE *action = item.action<AT_SET_QUEUE>();
				UInt8 queue = action->queue();
				io.mapRequired("queue", queue);
				break;
			}
			case OFPAT_GROUP: {
				const AT_GROUP *action = item.action<AT_GROUP>();
				UInt8 group = action->group();
				io.mapRequired("group", group);
				break;
			}
			case OFPAT_SET_NW_TTL: {
				const AT_SET_NW_TTL *action = item.action<AT_SET_NW_TTL>();
				UInt8 ttl = action->ttl();
				io.mapRequired("ttl", ttl);
				break;
			}
			case OFPAT_PUSH_PBB: {
				const AT_PUSH_PBB *action = item.action<AT_PUSH_PBB>();
				UInt16 ethertype = action->ethertype();
				io.mapRequired("ethertype", ethertype);
				break;
			}
			case OFPAT_EXPERIMENTER: {
				const AT_EXPERIMENTER *action = item.action<AT_EXPERIMENTER>();
				UInt32 experimenterid = action->experimenterid();
				io.mapRequired("experimenter_id", experimenterid);
				break;
			}
			case OFPAT_SET_FIELD: {
				OXMIterator iter = item.oxmIterator();
				OXMType oxmType = iter.type();
				io.mapRequired("type", oxmType);
				ofp::detail::OXMItemReader reader{io, RemoveConst_cast(*iter), oxmType};
        		OXMInternalID id = oxmType.internalID();
        		OXMDispatch(id, &reader);
				break;
			}
			default:
				log::info("MappingTraits: Unknown action type:", int(type));
				break;
    	}
    }
};

template <>
struct MappingTraits<ofp::detail::ActionInserter> {

    static void mapping(IO &io, ofp::detail::ActionInserter &builder)
    {
    	using namespace ofp;

    	ActionList &list = reinterpret_cast<ActionList&>(builder);

    	OFPActionType type;
    	io.mapRequired("action", type);
    	switch (type)
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
			case OFPAT_POP_MPLS: {
				AT_POP_MPLS action;
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
			case OFPAT_SET_QUEUE: {
				UInt8 queue;
				io.mapRequired("queue", queue);
				AT_SET_QUEUE action{queue};
				list.add(action);
				break;
			}
			case OFPAT_GROUP: {
				UInt8 group;
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
				io.mapRequired("experimenter_id", experimenterid);
				AT_EXPERIMENTER action{experimenterid};
				list.add(action);
				break;
			}
			case OFPAT_SET_FIELD: {
				OXMType oxmType;
				io.mapRequired("type", oxmType);
				ofp::OXMInternalID id = oxmType.internalID();
        		if (id != ofp::OXMInternalID::UNKNOWN) {
            		ofp::detail::SetFieldInserter inserter{io, list, oxmType};
            		OXMDispatch(id, &inserter);
        		}
				break;
			}
			default:
				log::info("MappingTraits<Inserter>: Unknown Action type:", int(type));
				break;
    	}
    }
};


template <>
struct SequenceTraits<ofp::ActionRange> {

    static size_t size(IO &io, ofp::ActionRange &actions)
    {
        return actions.itemCount();
    }

    static ofp::ActionIterator::Item &element(IO &io, ofp::ActionRange &actions,
                                     size_t index)
    {
        ofp::log::debug("action yaml item", index);
        // FIXME
        ofp::ActionIterator iter = actions.begin();
        for (size_t i = 0; i < index; ++i)
            ++iter;
        return RemoveConst_cast(*iter);
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
        return reinterpret_cast<ofp::detail::ActionInserter &>(list);
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YACTIONS_H
