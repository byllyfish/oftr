// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/prerequisites.h"
#include "ofp/log.h"
#include "ofp/oxmlist.h"
#include "ofp/oxmtypeset.h"

using namespace ofp;

// Use an OXMType that can't possibly exist to signal placeholders
// for prerequisites. We assume an OXMType with the mask bit set, but
// a zero length will not legally appear in the wire protocol.

constexpr const OXMType kMaskedPrereqSignal = OXMType(0x7FF1, 0, 0).withMask();
constexpr const OXMType kValuePrereqSignal = OXMType(0x7FF2, 0, 0).withMask();
constexpr const OXMType kPoisonPrereqSignal = OXMType(0x7FF3, 0, 0).withMask();

void Prerequisites::insertAll(OXMList *list, const OXMRange *preqs) {
  if (preqs != nullptr) {
    Prerequisites p{preqs};
    p.insert(list);
  }
}

void Prerequisites::insert(OXMList *list) const {
  assert(list != nullptr);

  // Insert prerequisites into the given list. We take advantage of the fact
  // that the prerequisites are already topologically sorted.
  // FIXME: Make sure insert can't loop indefinitely.

  OXMIterator preq = preqs_->begin();
  OXMIterator preqEnd = preqs_->end();

  while (preq != preqEnd) {
    OXMType preqType = preq->type();

    if (preqType.hasMask()) {
      insertPreqMasked(preqType, preq, list);
      advancePreq(preqType, preq, preqEnd);
    } else {
      insertPreqValue(preqType, preq, preqEnd, list);
    }
  }
}

void Prerequisites::insertPreqMasked(OXMType preqTypeMasked, OXMIterator preq,
                                     OXMList *list) const {
  if (!checkPreqMasked(preqTypeMasked, preq, list->begin(), list->end())) {
    // We add the masked prereq with a signal.
    log_info("MaskedPrereqSignal!!");
    list->addSignal(kMaskedPrereqSignal);
    list->add(preq);
  }
}

// Advances the preq.
void Prerequisites::insertPreqValue(OXMType preqType, OXMIterator &preq,
                                    OXMIterator preqEnd, OXMList *list) const {
  bool conflict = false;

  if (!checkPreqValue(preqType, preq, preqEnd, list->begin(), list->end(),
                      &conflict)) {
    // If there is only one value preq, insert it without the preq signal.
    // Otherwise, we need to insert the signal to indicate the presence of
    // multiple preq values.

    OXMIterator next = preq;
    ++next;
    if (next != preqEnd && next->type() == preqType) {
      // There are multiple preq values. Add the first preq with the
      // signal, then insert the remaining preqs.
      list->addSignal(kValuePrereqSignal);
      list->add(preq);
      list->add(next);
      preq = next;

      ++preq;
      while (preq != preqEnd && preq->type() == preqType) {
        list->add(preq);
        ++preq;
      }

    } else {
      // There is only one preq value. Insert it without a signal, if
      // there was no value conflict. Otherwise, add poison.
      if (!conflict) {
        list->add(preq);
      } else {
        list->addSignal(kPoisonPrereqSignal);
      }
      preq = next;
    }

  } else {
    advancePreq(preqType, preq, preqEnd);
  }
}

bool Prerequisites::checkAll(const OXMRange &oxm, FailureReason *reason) {
  // For each OXM entry, we will check if its prerequisites are met.
  // Complexity: If there are n items and each item has m prerequisites, this
  // algorithm is O(n*m).
  //
  // FIXME: Make sure this check can't loop indefinitely.
  // TODO(bfish): Improve algorithm and/or data structures.

  for (auto &item : oxm) {
    auto type = item.type();

    // Check for a type of length 0, but has the mask bit set. This is
    // treated as an illegal combination.

    if (type.isIllegal()) {
      log_info("Illegal oxm type ", log::hex(type));
      if (type == kValuePrereqSignal || type == kMaskedPrereqSignal) {
        *reason = kUnresolvedAmbiguity;
      } else if (type == kPoisonPrereqSignal) {
        *reason = kConflictingPrerequisites;
      }
      goto FAILURE;
    }

    // Skip experimenter types. Don't check prerequisites.
    if (type.isExperimenter()) {
      continue;
    }

    // Look up information record for the oxm type.
    auto typeInfo = type.lookupInfo();
    if (typeInfo == nullptr) {
      // Unrecognized oxm type. No way to check prerequisites. Skip it.
      log_info("Unknown oxm type ", log::hex(type));
      continue;
    }

    assert(typeInfo != nullptr);

    if (type.hasMask()) {
      // The oxm type has a mask. Check if it's allowed to have one.
      if (!typeInfo->isMaskSupported) {
        log_info("Invalid mask for ", log::hex(type));
        *reason = kInvalidMaskPresent;
        goto FAILURE;
      }
    }

    assert(!type.hasMask() || typeInfo->isMaskSupported);

    // Check if oxm type has prerequisites, and if so, check them all.
    if (typeInfo->prerequisites != nullptr) {
      Prerequisites preqs{typeInfo->prerequisites};
      if (!preqs.check(oxm.begin(), item.position())) {
        log_info("Prerequisite check failed for type ", log::hex(type));
        *reason = kMissingPrerequisite;
        goto FAILURE;
      }
    }
  }

  return true;

FAILURE:
  log_info("Prerequisite check failed for ", oxm);
  return false;
}

bool Prerequisites::check(OXMIterator begin, OXMIterator end) const {
  // Check this oxm range against our sequence of prerequisites.
  assert(begin <= end);

  OXMIterator preq = preqs_->begin();
  OXMIterator preqEnd = preqs_->end();
  bool conflict = false;

  while (preq < preqEnd) {
    OXMType preqType = preq->type();

    if (preqType.hasMask()) {
      if (checkPreqMasked(preqType, preq, begin, end))
        return true;
      advancePreq(preqType, preq, preqEnd);

    } else {
      if (checkPreqValue(preqType, preq, preqEnd, begin, end, &conflict))
        return true;
      ++preq;  // TODO(bfish):
    }
  }

  assert(preq == preqEnd);

  return false;
}

bool Prerequisites::checkPreqMasked(OXMType preqTypeMasked, OXMIterator preq,
                                    OXMIterator begin, OXMIterator end) const {
  assert(preqTypeMasked.hasMask());
  assert(begin <= end);

  OXMType preqType = preqTypeMasked.withoutMask();
  size_t valueLength = preqType.length();

  // Search for any values that match the preq. If we find one, return true.
  OXMIterator pos = begin;
  while (pos != end) {
    OXMType posType = pos->type();
    if (posType == preqType) {
      // Match `pos` value against prerequisite's value & mask.
      if (matchValueWithMask(valueLength, pos, preq)) {
        return true;
      }

    } else if (posType == preqTypeMasked) {
      // Match `pos` value & mask against prerequisite's value & mask.
      if (matchMaskWithMask(valueLength, pos, preq)) {
        return true;
      }
    } else {
      // Type doesn't match preq. Keep going.
    }
    ++pos;
  }

  return false;
}

bool Prerequisites::checkPreqValue(OXMType preqType, OXMIterator preqBegin,
                                   OXMIterator preqEnd, OXMIterator begin,
                                   OXMIterator end, bool *conflict) const {
  assert(!preqType.hasMask());
  assert(conflict != nullptr);
  assert(preqBegin <= preqEnd);
  assert(begin <= end);

  OXMType preqTypeMasked = preqType.withMask();
  size_t valueLength = preqType.length();

  // Search for any values that match the preq. If we find one, return true.
  OXMIterator pos = begin;
  while (pos < end) {
    OXMType posType = pos->type();

    OXMIterator preq = preqBegin;
    while (true) {
      if (posType == preqType) {
        // Match `pos` value against prerequisite's value.
        if (matchValueWithValue(valueLength, pos, preq)) {
          return true;
        }

      } else if (posType == preqTypeMasked) {
        // Match `pos` value & mask against prerequisite's value.
        if (matchMaskWithValue(valueLength, pos, preq)) {
          return true;
        }
      } else {
        // Type doesn't match preq. Try again with next element.
        break;
      }

      // Type matched preq, but value didn't. Try again with next preq
      // value of same type, if there is one.
      *conflict = true;
      ++preq;
      if (preq == preqEnd || preq->type() != preqType) {
        break;
      }
    }

    ++pos;
  }

  assert(pos == end);

  return false;
}

bool Prerequisites::matchValueWithMask(size_t length, OXMIterator pos,
                                       OXMIterator preq) {
  assert(length == pos->type().length());

  return matchValueWithMask(length, pos->unknownValuePtr(), preq);
}

bool Prerequisites::matchValueWithMask(size_t length, const void *data,
                                       OXMIterator preq) {
  assert(2 * length == preq->type().length());

  // Return true if the value matches the value & mask in the preq.

  const UInt8 *value = static_cast<const UInt8 *>(data);
  const UInt8 *preqValue = preq->unknownValuePtr();
  const UInt8 *preqMask = preqValue + length;

  for (size_t i = 0; i < length; ++i) {
    if ((*value++ & *preqMask++) != *preqValue++)
      return false;
  }

  return true;
}

bool Prerequisites::matchMaskWithMask(size_t length, OXMIterator pos,
                                      OXMIterator preq) {
  assert(2 * length == pos->type().length());
  assert(2 * length == preq->type().length());

  // Return true if masked value fits inside preq's mask.

  const UInt8 *value = pos->unknownValuePtr();
  const UInt8 *mask = value + length;
  const UInt8 *preqValue = preq->unknownValuePtr();
  const UInt8 *preqMask = preqValue + length;

  for (size_t i = 0; i < length; ++i) {
    UInt8 maskedValue = (*value++ & *mask++);
    if ((maskedValue & *preqMask++) != *preqValue++) {
      return false;
    }
  }

  return true;
}

bool Prerequisites::matchValueWithValue(size_t length, OXMIterator pos,
                                        OXMIterator preq) {
  assert(length == pos->type().length());

  return matchValueWithValue(length, pos->unknownValuePtr(), preq);
}

bool Prerequisites::matchValueWithValue(size_t length, const void *data,
                                        OXMIterator preq) {
  assert(length == preq->type().length() || preq->experimenter() != 0);

  // Return true if values are identical.
  return std::memcmp(data, preq->unknownValuePtr(), length) == 0;
}

bool Prerequisites::matchMaskWithValue(size_t length, OXMIterator pos,
                                       OXMIterator preq) {
  assert(2 * length == pos->type().length());
  assert(length == preq->type().length());

  // Return true if pos mask is all ones and values are identical.

  const UInt8 *value = pos->unknownValuePtr();
  const UInt8 *mask = value + length;
  const UInt8 *preqValue = preq->unknownValuePtr();

  for (size_t i = 0; i < length; ++i) {
    if ((*mask++ != 0xFF) || (*value++ != *preqValue++)) {
      return false;
    }
  }

  return true;
}

bool Prerequisites::substitute(OXMList *list, OXMType type, const void *value,
                               size_t len) {
  // Substitute value for prereq placeholder if we find one that matches the
  // type.
  assert(!type.hasMask());

  if (type.isExperimenter()) {
    return false;
  }

  OXMIterator iter = list->begin();
  OXMIterator iterEnd = list->end();

  while (iter != iterEnd) {
    auto escType = iter->type();

    if (escType == type) {
      // If type is already in the list. Check if value conflicts.
      if (!matchValueWithValue(len, value, iter)) {
        log_info("Value conflict detected in substitute; insert poison.", type);
        list->insertSignal(iter, kPoisonPrereqSignal);
      }
      return true;
    }

    if (escType != kMaskedPrereqSignal && escType != kValuePrereqSignal) {
      ++iter;
      continue;
    }

    OXMIterator start = iter;
    ++iter;
    if (iter == iterEnd) {
      break;
    }

    auto preqType = iter->type();
    if (escType == kMaskedPrereqSignal) {
      assert(preqType.hasMask());

      if (preqType.withoutMask() == type &&
          matchValueWithMask(len, value, iter)) {
        ++iter;
        // Perform substitution into OXMList.
        OXMIterator rest = list->replace(start, iter, type, value, len);
        // Check for duplicate items afer the inserted item.
        poisonDuplicatesAfterSubstitution(list, type, rest);
        return true;
      }
      ++iter;

    } else {
      assert(escType == kValuePrereqSignal);
      assert(!preqType.hasMask());

      while (preqType == type) {
        if (matchValueWithValue(len, value, iter)) {
          advancePreq(preqType, iter, iterEnd);
          // Perform substituion into OXMList.
          OXMIterator rest = list->replace(start, iter, type, value, len);
          // Check for duplicate items after the inserted item.
          poisonDuplicatesAfterSubstitution(list, type, rest);
          return true;
        }
        ++iter;
        if (iter == iterEnd)
          break;
      }
    }
  }

  return false;
}

void Prerequisites::poisonDuplicatesAfterSubstitution(OXMList *list,
                                                      OXMType type,
                                                      OXMIterator rest) {
  OXMIterator restEnd = list->end();
  while (rest != restEnd) {
    if (rest->type() == type) {
      log_error("Duplicate item detected after substitution. Insert poison.");
      list->insertSignal(rest, kPoisonPrereqSignal);
      break;
    }
    ++rest;
  }
}

bool Prerequisites::substitute(OXMList *list, OXMType type, const void *value,
                               const void *mask, size_t len) {
  // FIXME
  return false;
}

/// Check for the presence of duplicate fields in the oxm list.
bool Prerequisites::duplicateFieldsDetected(const OXMRange &oxm) {
  OXMTypeSet typeSet;

  for (auto &item : oxm) {
    OXMType type = item.type();
    if (!typeSet.add(type)) {
      log_info("Duplicate field detected: ", type);
      return true;
    }
  }

  return false;
}

void Prerequisites::advancePreq(OXMType preqType, OXMIterator &preq,
                                OXMIterator preqEnd) {
  ++preq;
  while (preq != preqEnd && preq->type() == preqType) {
    ++preq;
  }
}
