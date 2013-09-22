#ifndef OFP_YAML_YINSTRUCTIONS_H
#define OFP_YAML_YINSTRUCTIONS_H

#include "ofp/instructionlist.h"
#include "ofp/yaml/ybyteorder.h"
#include "ofp/yaml/yactions.h"
#include "ofp/instructionrange.h"

namespace ofp { // <namespace ofp>
namespace detail { // <namespace detail>

struct InstructionInserter 
{
};

} // </namespace detail>
} // </namespace ofp>

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

// Pointers are used for read-only.
template <>
struct MappingTraits<ofp::IT_GOTO_TABLE*> {
    static void mapping(IO &io, ofp::IT_GOTO_TABLE *&instr)
    {
        io.mapRequired("table_id", instr->tableId_);
    }
};

template <>
struct MappingTraits<ofp::IT_GOTO_TABLE> {
    static void mapping(IO &io, ofp::IT_GOTO_TABLE &instr)
    {
    	io.mapRequired("table_id", instr.tableId_);
    }
};

template <>
struct MappingTraits<ofp::IT_WRITE_METADATA*> {
    static void mapping(IO &io, ofp::IT_WRITE_METADATA *&instr)
    {
        io.mapRequired("metadata", instr->metadata_);
        io.mapRequired("mask", instr->mask_);
    }
};

template <>
struct MappingTraits<ofp::IT_WRITE_METADATA> {
    static void mapping(IO &io, ofp::IT_WRITE_METADATA &instr)
    {
    	io.mapRequired("metadata", instr.metadata_);
    	io.mapRequired("mask", instr.mask_);
    }
};

#if 0
template <>
struct MappingTraits<ofp::IT_WRITE_ACTIONS> {
    static void mapping(IO &io, ofp::IT_WRITE_ACTIONS &instr)
    {
    	//FIXME io.mapRequired("actions", *instr.actions_);
    }
};

template <>
struct MappingTraits<ofp::IT_APPLY_ACTIONS> {
    static void mapping(IO &io, ofp::IT_APPLY_ACTIONS &instr)
    {
    	//FIXME io.mapRequired("actions", *instr.actions_);
    }
};


template <>
struct MappingTraits<ofp::IT_CLEAR_ACTIONS> {
    static void mapping(IO &io, ofp::IT_CLEAR_ACTIONS &instr)
    {
    }
};
#endif //0

template <>
struct MappingTraits<ofp::IT_METER*> {
    static void mapping(IO &io, ofp::IT_METER *&instr)
    {
        io.mapRequired("meter", instr->meter_);
    }
};

template <>
struct MappingTraits<ofp::IT_METER> {
    static void mapping(IO &io, ofp::IT_METER &instr)
    {
    	io.mapRequired("meter", instr.meter_);
    }
};

template <>
struct MappingTraits<ofp::IT_EXPERIMENTER*> {
    static void mapping(IO &io, ofp::IT_EXPERIMENTER *&instr)
    {
        io.mapRequired("meter", instr->experimenterId_);
    }
};

template <>
struct MappingTraits<ofp::IT_EXPERIMENTER> {
    static void mapping(IO &io, ofp::IT_EXPERIMENTER &instr)
    {
    	io.mapRequired("meter", instr.experimenterId_);
    }
};


template <>
struct MappingTraits<ofp::InstructionIterator::Item> {

    static void mapping(IO &io, ofp::InstructionIterator::Item &item)
    {
        using namespace ofp;

        OFPInstructionType type = item.type();
        io.mapRequired("type", type);
        switch (type) 
        {
            case IT_GOTO_TABLE::type(): {
                IT_GOTO_TABLE *instr = RemoveConst_cast(item.instruction<IT_GOTO_TABLE>());
                io.mapRequired("value", instr);
                break;
            }
            case IT_WRITE_METADATA::type(): {
                IT_WRITE_METADATA *instr = RemoveConst_cast(item.instruction<IT_WRITE_METADATA>());
                io.mapRequired("value", instr);
                break;
            }
            case IT_WRITE_ACTIONS::type(): {
                IT_WRITE_ACTIONS *instr = RemoveConst_cast(item.instruction<IT_WRITE_ACTIONS>());
                ActionRange actions{instr->dataRange()};
                io.mapRequired("value", actions);
                break;
            }
            case IT_APPLY_ACTIONS::type(): {
                IT_APPLY_ACTIONS *instr = RemoveConst_cast(item.instruction<IT_APPLY_ACTIONS>());
                ActionRange actions{instr->dataRange()};
                io.mapRequired("value", actions);
                break;
            }
            case IT_CLEAR_ACTIONS::type(): {
                // No value.
                break;
            }
            case IT_METER::type(): {
                IT_METER *instr = RemoveConst_cast(item.instruction<IT_METER>());
                io.mapRequired("value", instr);
                break;
            }
            case IT_EXPERIMENTER::type(): {
                IT_EXPERIMENTER *instr = RemoveConst_cast(item.instruction<IT_EXPERIMENTER>());
                io.mapRequired("value", instr);
                break;
            }
        }
    }
};

template <>
struct MappingTraits<ofp::detail::InstructionInserter> {

    static void mapping(IO &io, ofp::detail::InstructionInserter &builder)
    {
    	using namespace ofp;

    	InstructionList &list = reinterpret_cast<InstructionList &>(builder);

    	OFPInstructionType type;
    	io.mapRequired("type", type);
    	switch (type) 
    	{
    		case IT_GOTO_TABLE::type(): {
    			IT_GOTO_TABLE instr{0};
    			io.mapRequired("value", instr);
    			list.add(instr);
    			break;
    		}
    		case IT_WRITE_METADATA::type(): {
    			IT_WRITE_METADATA instr{0,0};
    			io.mapRequired("value", instr);
    			list.add(instr);
    			break;
    		}
    		case IT_WRITE_ACTIONS::type(): {
    			ActionList actions;
    			io.mapRequired("value", actions);
    			list.add(IT_WRITE_ACTIONS{&actions});
    			break;
    		}
    		case IT_APPLY_ACTIONS::type(): {
    			ActionList actions;
    			io.mapRequired("value", actions);
    			list.add(IT_APPLY_ACTIONS{&actions});
    			break;
    		}
    		case IT_CLEAR_ACTIONS::type(): {
    			IT_CLEAR_ACTIONS instr;
    			//io.mapRequired("value", instr);
    			list.add(instr);
    			break;
    		}
    		case IT_METER::type(): {
    			IT_METER instr{0};
    			io.mapRequired("value", instr);
    			list.add(instr);
    			break;
    		}
    		case IT_EXPERIMENTER::type(): {
    			IT_EXPERIMENTER instr{0};
    			io.mapRequired("value", instr);
    			list.add(instr);
    			break;
    		}
    	}
    }
};


template <>
struct SequenceTraits<ofp::InstructionRange> {

    static size_t size(IO &io, ofp::InstructionRange &range)
    {
        ofp::log::debug("Sequence::INstructionRange: ", ofp::RawDataToHex(range.data(), range.size()));
        return range.itemCount();
    }

    static ofp::InstructionIterator::Item &element(IO &io, ofp::InstructionRange &range,
                                     size_t index)
    {
        ofp::log::debug("instruction yaml item", index);
        // FIXME - iterates every time!
        ofp::InstructionIterator iter = range.begin();
        for (size_t i = 0; i < index; ++i)
            ++iter;
        return RemoveConst_cast(*iter);
    }
};

template <>
struct SequenceTraits<ofp::InstructionList> {

    static size_t size(IO &io, ofp::InstructionList &list)
    {
        return 0;
    }

    static ofp::detail::InstructionInserter &element(IO &io, ofp::InstructionList &list,
                                     size_t index)
    {
        return reinterpret_cast<ofp::detail::InstructionInserter &>(list);
    }
};



} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YINSTRUCTIONS_H
