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

#ifndef OFP_MPDESC_H
#define OFP_MPDESC_H

#include "ofp/smallcstring.h"

namespace ofp { // <namespace ofp>

class MPDesc {
public:
  using DescStr = SmallCString<256>;
  using SerialNumStr = SmallCString<32>;

  std::string mfrDesc() const { return mfrDesc_.toString(); }
  void setMfrDesc(const std::string &mfrDesc) { mfrDesc_ = mfrDesc; }

  std::string hwDesc() const { return hwDesc_.toString(); }
  void setHwDesc(const std::string &hwDesc) { hwDesc_ = hwDesc; }

  std::string swDesc() const { return swDesc_.toString(); }
  void setswDesc(const std::string &swDesc) { swDesc_ = swDesc; }

  std::string serialNum() const { return serialNum_.toString(); }
  void setSerialNum(const std::string &serialNum) { serialNum_ = serialNum; }

  std::string dpDesc() const { return dpDesc_.toString(); }
  void setDpDesc(const std::string &dpDesc) { dpDesc_ = dpDesc; }

  bool validateLength(size_t length) const { return length == 1056; }

private:
  DescStr mfrDesc_{};        // Manufacturer description
  DescStr hwDesc_{};         // Hardware description
  DescStr swDesc_{};         // Software description
  SerialNumStr serialNum_{}; // Serial number
  DescStr dpDesc_{};         // Human readable description of datapath

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPDesc::DescStr) == 256, "Unexpected size.");
static_assert(IsStandardLayout<MPDesc::DescStr>(), "Expected standard layout.");
static_assert(sizeof(MPDesc::SerialNumStr) == 32, "Unexpected size.");
static_assert(IsStandardLayout<MPDesc::SerialNumStr>(), "Expected standard layout.");
static_assert(sizeof(MPDesc) == 1056, "Unexpected size.");
static_assert(IsStandardLayout<MPDesc>(), "Expected standard layout.");

} // </namespace ofp>

#endif // OFP_MPDESC_H
