// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YMATCH_H_
#define OFP_YAML_YMATCH_H_

#include "ofp/match.h"
#include "ofp/matchbuilder.h"
#include "ofp/yaml/encoder.h"
#include "ofp/yaml/seterror.h"
#include "ofp/yaml/yaddress.h"
#include "ofp/yaml/ybytelist.h"
#include "ofp/yaml/ybyteorder.h"
#include "ofp/yaml/ydurationsec.h"
#include "ofp/yaml/ylldpvalue.h"
#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/yoxmfulltype.h"
#include "ofp/yaml/yportnumber.h"
#include "ofp/yaml/yvlannumber.h"

namespace ofp {
namespace detail {

OFP_BEGIN_IGNORE_PADDING

class OXMItemReader {
 public:
  OXMItemReader(llvm::yaml::IO &io, OXMIterator::Item &item, OXMFullType type)
      : io_(io), item_(item), type_{type} {}

  template <class ValueType>
  void visit() {
    auto val = item_.value<ValueType>().valueOriginal();
    io_.mapRequired("value", val);
    if (type_.hasMask()) {
      auto mask = item_.mask<ValueType>().valueOriginal();
      io_.mapRequired("mask", mask);
    }
  }

 private:
  llvm::yaml::IO &io_;
  OXMIterator::Item &item_;
  OXMFullType type_;
};

class MatchBuilderInserter {
 public:
  MatchBuilderInserter(llvm::yaml::IO &io, MatchBuilder &builder,
                       OXMFullType type)
      : io_(io), builder_(builder), type_{type} {
    yaml::Encoder *encoder = yaml::GetEncoderFromContext(io);
    if (encoder) {
      checkPrereqs_ = encoder->matchPrereqsChecked();
    }
  }

  template <class ValueType>
  void visit() {
    assert(!type_.hasMask());

    typename ValueType::NativeType value;
    io_.mapRequired("value", value);

    llvm::Optional<decltype(value)> optMask;
    io_.mapOptional("mask", optMask);

    if (optMask.hasValue()) {
      if (checkPrereqs_) {
        builder_.add(ValueType{value}, ValueType{*optMask});
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
  OXMFullType type_;
  bool checkPrereqs_ = true;
};

OFP_END_IGNORE_PADDING

struct MatchBuilderItem {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct SequenceTraits<ofp::Match> {
  using iterator = ofp::OXMIterator;

  static iterator begin(IO &io, ofp::Match &match) { return match.begin(); }

  static iterator end(IO &io, ofp::Match &match) { return match.end(); }

  static void next(iterator &iter, iterator iterEnd) { ++iter; }
};

template <>
struct MappingTraits<ofp::OXMIterator::Item> {
  static void mapping(IO &io, ofp::OXMIterator::Item &item) {
    ofp::OXMFullType type{item.type(), item.experimenter()};
    io.mapRequired("field", type);

    ofp::detail::OXMItemReader reader{io, item, type};
    ofp::OXMInternalID id = type.internalID();

    if (id != ofp::OXMInternalID::UNKNOWN) {
      OXMDispatch(id, &reader);
    } else {
      log_debug("MappingTraitss<OXMIterator::Item>: Unrecognized match field",
                type);
      ofp::ByteRange data{item.unknownValuePtr(), item.unknownValueLength()};
      if (type.hasMask()) {
        // First half is value, second half is mask.
        auto pair = splitRangeInHalf(data);
        io.mapRequired("value", pair.first);
        io.mapRequired("mask", pair.second);

      } else {
        io.mapRequired("value", data);
      }
    }
  }

 private:
  static std::pair<ofp::ByteRange, ofp::ByteRange> splitRangeInHalf(
      const ofp::ByteRange &data) {
    // Split range in half (approximately, if size is odd).
    ofp::ByteRange first{data.data(), data.size() / 2};
    ofp::ByteRange second{data.data() + first.size(),
                          data.size() - first.size()};
    return {first, second};
  }
};

template <>
struct SequenceTraits<ofp::MatchBuilder> {
  static size_t size(IO &io, ofp::MatchBuilder &match) { return 0; }

  static ofp::detail::MatchBuilderItem &element(IO &io,
                                                ofp::MatchBuilder &match,
                                                size_t index) {
    return Ref_cast<ofp::detail::MatchBuilderItem>(match);
  }

  static StringRef validate(IO &io, ofp::MatchBuilder &match) {
    if (ofp::yaml::ErrorFound(io)) {
      // Don't add another error if there is already one.
      return "";
    }

    ofp::yaml::Encoder *encoder = ofp::yaml::GetEncoderFromContext(io);
    if (encoder && encoder->matchPrereqsChecked()) {
      ofp::Prerequisites::FailureReason reason;
      if (!match.validate(&reason)) {
        switch (reason) {
          case ofp::Prerequisites::kUnresolvedAmbiguity:
            return "Invalid match: Missing prerequisites";
          case ofp::Prerequisites::kConflictingPrerequisites:
            return "Invalid match: Conflicting prerequisites";
          default:
            return "Invalid match";
        }
      }
    }
    return "";
  }
};

template <>
struct MappingTraits<ofp::detail::MatchBuilderItem> {
  static void mapping(IO &io, ofp::detail::MatchBuilderItem &item) {
    ofp::MatchBuilder &builder = Ref_cast<ofp::MatchBuilder>(item);

    ofp::OXMFullType type;
    io.mapRequired("field", type);

    ofp::OXMInternalID id = type.internalID();
    if (id != ofp::OXMInternalID::UNKNOWN) {
      ofp::detail::MatchBuilderInserter inserter{io, builder, type};
      OXMDispatch(id, &inserter);

    } else {
      addUnexpected(io, builder, type);
    }
  }

  static void addUnexpected(IO &io, ofp::MatchBuilder &builder,
                            const ofp::OXMFullType &type) {
    log_debug("MappingTraits<MatchBuilderItem>: Unexpected match field", type);
    ofp::ByteList data;
    io.mapRequired("value", data);

    if (!type.hasMask()) {
      size_t len = data.size() + (type.experimenter() != 0 ? 4 : 0);
      if (len == type.length()) {
        builder.addUnchecked(type.type(), type.experimenter(), data);
      } else {
        io.setError("Invalid field length");
        log_debug("Invalid data size:", type);
      }
    } else {
      // Handle mask in unknown OXM field value.
      ofp::ByteList mask;
      io.mapRequired("mask", mask);

      if (data.size() != mask.size()) {
        io.setError("Unequal field mask size");
        log_debug("Mask size does not equal data size");
        return;
      }

      size_t len = 2 * data.size() + (type.experimenter() != 0 ? 4 : 0);
      if (len == type.length()) {
        builder.addUnchecked(type.type(), type.experimenter(), data, mask);
      } else {
        io.setError("Invalid field length");
        log_debug("Invalid data size:", type);
      }
    }
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMATCH_H_
