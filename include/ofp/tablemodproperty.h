// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_TABLEMODPROPERTY_H_
#define OFP_TABLEMODPROPERTY_H_

#include "ofp/constants.h"
#include "ofp/experimenterproperty.h"
#include "ofp/padding.h"

namespace ofp {

class PropertyRange;
class Validation;

// TableModPropertyEviction needs to be `Copyable` since it is used as an
// optional property.

class TableModPropertyEviction {
 public:
  constexpr static OFPTableModProperty type() { return OFPTMPT_EVICTION; }

  void setFlags(UInt32 flags) { flags_ = flags; }

  bool validateInput(Validation *context) const { return len_ == 8; }

 private:
  Big16 type_ = type();
  Big16 len_ = 8;
  Big32 flags_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(TableModPropertyEviction) == 8, "Unexpected size");
static_assert(IsStandardLayout<TableModPropertyEviction>(),
              "Expected standard layout.");

// TableModPropertyVacancy needs to be `Copyable` since it is used as an
// optional property.

class TableModPropertyVacancy {
 public:
  constexpr static OFPTableModProperty type() { return OFPTMPT_VACANCY; }

  void setVacancyDown(UInt8 vacancyDown) { vacancyDown_ = vacancyDown; }
  void setVacancyUp(UInt8 vacancyUp) { vacancyUp_ = vacancyUp; }
  void setVacancy(UInt8 vacancy) { vacancy_ = vacancy; }

  bool validateInput(Validation *context) const { return len_ == 8; }

 private:
  Big16 type_ = type();
  Big16 len_ = 8;
  Big8 vacancyDown_;
  Big8 vacancyUp_;
  Big8 vacancy_;
  Padding<1> pad_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(TableModPropertyVacancy) == 8, "Unexpected size");
static_assert(IsStandardLayout<TableModPropertyVacancy>(),
              "Expected standard layout.");

using TableModPropertyExperimenter =
    detail::ExperimenterProperty<OFPTableModProperty, OFPTMPT_EXPERIMENTER>;

class TableModPropertyValidator {
 public:
  static bool validateInput(const PropertyRange &range, Validation *context);
};

}  // namespace ofp

#endif  // OFP_TABLEMODPROPERTY_H_
