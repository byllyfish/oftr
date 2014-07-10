//  ===== ---- ofp/yaml/ymatch.h ---------------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::SequenceTraits for the Match and
/// MatchBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YMATCH_H_
#define OFP_YAML_YMATCH_H_

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ybyteorder.h"
#include "ofp/yaml/yaddress.h"
#include "ofp/yaml/ybytelist.h"
#include "ofp/yaml/yoxmtype.h"
#include "ofp/match.h"
#include "ofp/matchbuilder.h"
#include "ofp/yaml/encoder.h"

namespace ofp {
namespace detail {

OFP_BEGIN_IGNORE_PADDING

class OXMItemReader {
public:
    OXMItemReader(llvm::yaml::IO &io, OXMIterator::Item &item, OXMType type)
        : io_(io), item_(item), type_{type}
    {
    }

    template <class ValueType>
    void visit()
    {
        auto val = item_.value<ValueType>().value();
        io_.mapRequired("value", val);
        if (type_.hasMask()) {
            auto mask = item_.mask<ValueType>().value();
            io_.mapRequired("mask", mask);
        }
    }

private:
    llvm::yaml::IO &io_;
    OXMIterator::Item &item_;
    OXMType type_;
};

class MatchBuilderInserter {
public:
    MatchBuilderInserter(llvm::yaml::IO &io, MatchBuilder &builder,
                         OXMType type)
        : io_(io), builder_(builder), type_{type}
    {
        yaml::Encoder *encoder = reinterpret_cast<yaml::Encoder *>(io.getContext());
        if (encoder) {
            checkPrereqs_ = encoder->matchPrereqsChecked();
        }
    }

    template <class ValueType>
    void visit()
    {
        assert(!type_.hasMask());

        typename ValueType::NativeType value;
        io_.mapRequired("value", value);

        llvm::Optional<decltype(value)> optMask;
        io_.mapOptional("mask", optMask);

        if (optMask.hasValue()) {
            if (checkPrereqs_) {
                if (!ValueType::maskSupported()) {
                    // TODO(bfish) better error message
                    io_.setError("Mask is not supported.");
                    ofp::log::info("Match is not supported:", type_);
                } else {
                    builder_.add(ValueType{value}, ValueType{*optMask});
                }
            } else {
                builder_.addUnchecked(ValueType{value}, ValueType{*optMask});
            }
        } else {
            if (checkPrereqs_) {
                builder_.add(ValueType{value});
            } else {
                builder_.addUnchecked(ValueType{value});
            }
        }
    }

private:
    llvm::yaml::IO &io_;
    MatchBuilder &builder_;
    OXMType type_;
    bool checkPrereqs_ = true;
};

OFP_END_IGNORE_PADDING

struct MatchBuilderItem {
};

}  // namespace detail
}  // namespace ofp


namespace llvm {
namespace yaml {


template <>
struct SequenceTraits<ofp::Match> {
    using iterator = ofp::OXMIterator;

    static iterator begin(IO &io, ofp::Match &match) {
        return match.begin();
    }

    static iterator end(IO &io, ofp::Match &match) {
        return match.end();
    }
};

template <>
struct MappingTraits<ofp::OXMIterator::Item> {

    static void mapping(IO &io, ofp::OXMIterator::Item &item)
    {
        ofp::OXMType type = item.type();
        io.mapRequired("field", type);

        ofp::detail::OXMItemReader reader{io, item, type};
        ofp::OXMInternalID id = type.internalID();

        if (id != ofp::OXMInternalID::UNKNOWN) {
            OXMDispatch(id, &reader);
        } else {
            ofp::ByteRange data{item.unknownValuePtr(), type.length()};
            io.mapRequired("value", data);
        }
    }
};


template <>
struct SequenceTraits<ofp::MatchBuilder> {

    static size_t size(IO &io, ofp::MatchBuilder &match)
    {
        return match.itemCount();
    }

    static ofp::detail::MatchBuilderItem &element(IO &io, ofp::MatchBuilder &match,
                                     size_t index)
    {
        return reinterpret_cast<ofp::detail::MatchBuilderItem &>(match);
    }
};

template <>
struct MappingTraits<ofp::detail::MatchBuilderItem> {

    static void mapping(IO &io, ofp::detail::MatchBuilderItem &item)
    {
        ofp::MatchBuilder &builder = reinterpret_cast<ofp::MatchBuilder &>(item);

        ofp::OXMType type;
        io.mapRequired("field", type);

        ofp::OXMInternalID id = type.internalID();
        if (id != ofp::OXMInternalID::UNKNOWN) {
            ofp::detail::MatchBuilderInserter inserter{io, builder, type};
            OXMDispatch(id, &inserter);

        } else {
            ofp::ByteList data;
            io.mapRequired("value", data);
            if (data.size() == type.length()) {
                builder.addUnchecked(type, data);
            } else {
                ofp::log::debug("Invalid data size:", type);
            }
            //ofp::log::debug("Unknown oxmtype: ", type);
        }
    }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMATCH_H_
