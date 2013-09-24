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

#ifndef OFP_YAML_YMATCH_H
#define OFP_YAML_YMATCH_H

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ybyteorder.h"
#include "ofp/yaml/yaddress.h"
#include "ofp/match.h"
#include "ofp/matchbuilder.h"

namespace ofp { // <namespace ofp>
namespace detail { // <namespace detail>

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
    }

    template <class ValueType>
    void visit()
    {
        if (type_.hasMask()) {
            typename ValueType::NativeType value;
            typename ValueType::NativeType mask;
            io_.mapRequired("value", value);
            io_.mapRequired("mask", mask);
            builder_.add(ValueType{value}, ValueType{mask});
        } else {
            typename ValueType::NativeType value;
            io_.mapRequired("value", value);
            builder_.add(ValueType{value});
        }
    }

private:
    llvm::yaml::IO &io_;
    MatchBuilder &builder_;
    OXMType type_;
};

OFP_END_IGNORE_PADDING

struct MatchBuilderItem {
};

} // </namespace detail>
} // </namespace ofp>


namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>


template <>
struct SequenceTraits<ofp::Match> {

    static size_t size(IO &io, ofp::Match &match)
    {
        return match.itemCount();
    }

    static ofp::OXMIterator::Item &element(IO &io, ofp::Match &match,
                                           size_t index)
    {
        ofp::log::debug("match yaml item", index);

        // FIXME
        ofp::OXMIterator iter = match.begin();
        for (size_t i = 0; i < index; ++i)
            ++iter;
        return RemoveConst_cast(*iter);
    }
};

template <>
struct MappingTraits<ofp::OXMIterator::Item> {

    static void mapping(IO &io, ofp::OXMIterator::Item &item)
    {
        ofp::OXMType type = item.type();
        io.mapRequired("type", type);

        ofp::detail::OXMItemReader reader{io, item, type};
        ofp::OXMInternalID id = type.internalID();
        OXMDispatch(id, &reader);
    }
};

template <>
struct ScalarTraits<ofp::OXMType> {
    static void output(const ofp::OXMType &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        const ofp::OXMTypeInfo *info = value.lookupInfo();
        if (info) {
            out << info->name;
        } else {
            out << value.oxmNative();
        }
    }

    static StringRef input(StringRef scalar, void *ctxt, ofp::OXMType &value)
    {
        if (!value.parse(scalar)) {
            return "Invalid OXM type.";
        }

        return "";
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
        io.mapRequired("type", type);

        ofp::OXMInternalID id = type.internalID();
        if (id != ofp::OXMInternalID::UNKNOWN) {
            ofp::detail::MatchBuilderInserter inserter{io, builder, type};
            OXMDispatch(id, &inserter);

        } else {
            ofp::log::debug("Unknown oxmtype: ", type);
        }
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YMATCH_H
