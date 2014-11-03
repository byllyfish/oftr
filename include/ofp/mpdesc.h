//  ===== ---- ofp/mpdesc.h --------------------------------*- C++ -*- =====  //
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
/// \brief Defines MPDesc class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_MPDESC_H_
#define OFP_MPDESC_H_

#include "ofp/strings.h"

namespace ofp {

class Writable;
class Validation;

class MPDesc {
 public:
  std::string mfrDesc() const { return mfrDesc_.toString(); }
  std::string hwDesc() const { return hwDesc_.toString(); }
  std::string swDesc() const { return swDesc_.toString(); }
  std::string serialNum() const { return serialNum_.toString(); }
  std::string dpDesc() const { return dpDesc_.toString(); }

  bool validateInput(Validation *context) const;

 private:
  DescriptionStr mfrDesc_{};     // Manufacturer description
  DescriptionStr hwDesc_{};      // Hardware description
  DescriptionStr swDesc_{};      // Software description
  SerialNumberStr serialNum_{};  // Serial number
  DescriptionStr dpDesc_{};      // Human readable description of datapath

  friend class MPDescBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPDesc) == 1056, "Unexpected size.");
static_assert(IsStandardLayout<MPDesc>(), "Expected standard layout.");

class MPDescBuilder {
 public:
  MPDescBuilder() = default;

  void setMfrDesc(const std::string &mfrDesc) { msg_.mfrDesc_ = mfrDesc; }
  void setHwDesc(const std::string &hwDesc) { msg_.hwDesc_ = hwDesc; }
  void setswDesc(const std::string &swDesc) { msg_.swDesc_ = swDesc; }
  void setSerialNum(const std::string &serialNum) {
    msg_.serialNum_ = serialNum;
  }
  void setDpDesc(const std::string &dpDesc) { msg_.dpDesc_ = dpDesc; }

  void write(Writable *channel);

 private:
  MPDesc msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPDESC_H_
