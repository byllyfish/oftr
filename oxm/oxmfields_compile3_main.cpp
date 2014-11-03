//  ===== ---- ofp/oxmfields_compile3_main.cpp -------------*- C++ -*- =====  //
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
/// \brief Program to sort OXM types by typeID and output C++ source file
/// `oxmfieldsdata.cpp`.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/oxmfields.h"
#include <vector>
#include <iostream>
#include <algorithm>

using namespace ofp;

int main() {
  std::vector<OXMTypeInternalMapEntry> entries;

  for (size_t i = 0; i < OXMTypeInfoArraySize; ++i) {
    OXMTypeInternalMapEntry entry;
    entry.value32 = OXMTypeInfoArray[i].value32;
    entry.id = static_cast<OXMInternalID>(i);
    entries.push_back(entry);
  }

  std::sort(entries.begin(), entries.end(),
            [](const OXMTypeInternalMapEntry &lhs,
               const OXMTypeInternalMapEntry &rhs) {
    return lhs.value32 < rhs.value32;
  });

  std::cout << R"""(//
#include "ofp/oxmfields.h"

)""";

  std::cout << "const ofp::OXMTypeInternalMapEntry "
               "ofp::OXMTypeInternalMapArray[] = {\n";
  for (auto entry : entries) {
    std::cout << "  { " << entry.value32 << ", ";
    std::cout << "static_cast<ofp::OXMInternalID>("
              << static_cast<int>(entry.id) << ") },\n";
  }
  std::cout << "};\n";
}
