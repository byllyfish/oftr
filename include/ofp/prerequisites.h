// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_PREREQUISITES_H_
#define OFP_PREREQUISITES_H_

#include "ofp/oxmtype.h"
#include "ofp/oxmrange.h"

namespace ofp {

class OXMList;

class Prerequisites {
 public:

  enum FailureReason {
    kUnresolvedAmbiguity = 1,
    kConflictingPrerequisites,
    kInvalidMaskPresent,
    kMissingPrerequisite,
    kDuplicateFieldsDetected
  };

  static void insertAll(OXMList *list, const OXMRange *preqs);
  static bool checkAll(const OXMRange &oxm, FailureReason *reason);

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
