//  ===== ---- ofp/prerequisites.h -------------------------*- C++ -*- =====  //
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
/// \brief Defines the Prerequisites utility class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_PREREQUISITES_H_
#define OFP_PREREQUISITES_H_

#include "ofp/oxmtype.h"
#include "ofp/oxmrange.h"

namespace ofp {

class OXMList;

class Prerequisites {
public:
  static void insertAll(OXMList *list, const OXMRange *preqs);
  static bool checkAll(const OXMRange &oxm);

  static bool substitute(OXMList *list, OXMType type, const void *value,
                         size_t len);
  static bool substitute(OXMList *list, OXMType type, const void *value,
                         const void *mask, size_t len);

  static bool duplicateFieldsDetected(const OXMRange &oxm);

private:
  const OXMRange *preqs_;

  explicit Prerequisites(const OXMRange *preqs) : preqs_{preqs} {
    assert(preqs != nullptr);
  }

  void insert(OXMList *list) const;
  bool check(OXMIterator begin, OXMIterator end) const;

  void insertPreqMasked(OXMType preqTypeMasked, OXMIterator preq,
                        OXMList *list) const;
  void insertPreqValue(OXMType preqType, OXMIterator &preq, OXMIterator preqEnd,
                       OXMList *list) const;

  bool checkPreqMasked(OXMType preqTypeMasked, OXMIterator preq,
                       OXMIterator begin, OXMIterator end) const;
  bool checkPreqValue(OXMType preqType, OXMIterator preq, OXMIterator preqEnd,
                      OXMIterator begin, OXMIterator end, bool *conflict) const;

  static bool matchValueWithMask(size_t length, OXMIterator pos,
                                 OXMIterator preq);
  static bool matchValueWithMask(size_t length, const void *data,
                                 OXMIterator preq);
  static bool matchMaskWithMask(size_t length, OXMIterator pos,
                                OXMIterator preq);
  static bool matchValueWithValue(size_t length, OXMIterator pos,
                                  OXMIterator preq);
  static bool matchValueWithValue(size_t length, const void *data,
                                  OXMIterator preq);
  static bool matchMaskWithValue(size_t length, OXMIterator pos,
                                 OXMIterator preq);

  static void advancePreq(OXMType preqType, OXMIterator &preq,
                          OXMIterator preqEnd);

  static void poisonDuplicatesAfterSubstitution(OXMList *list, OXMType type,
                                                OXMIterator rest);
};

}  // namespace ofp

#endif  // OFP_PREREQUISITES_H_
