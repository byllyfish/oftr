// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/actiontype.h"
#include "ofp/actions.h"

using namespace ofp;

static const ActionTypeInfo sActionInfo[] = {
    {AT_OUTPUT::type(), "OUTPUT", 0, 0},
    {AT_COPY_TTL_OUT::type(), "COPY_TTL_OUT", 0, 0},
    {AT_COPY_TTL_IN::type(), "COPY_TTL_IN", 0, 0},
    {AT_SET_MPLS_TTL::type(), "SET_MPLS_TTL", 0, 0},
    {AT_DEC_MPLS_TTL::type(), "DEC_MPLS_TTL", 0, 0},
    {AT_PUSH_VLAN::type(), "PUSH_VLAN", 0, 0},
    {AT_POP_VLAN::type(), "POP_VLAN", 0, 0},
    {AT_PUSH_MPLS::type(), "PUSH_MPLS", 0, 0},
    {AT_POP_MPLS::type(), "POP_MPLS", 0, 0},
    {AT_SET_QUEUE::type(), "SET_QUEUE", 0, 0},
    {AT_GROUP::type(), "GROUP", 0, 0},
    {AT_SET_NW_TTL::type(), "SET_NW_TTL", 0, 0},
    {AT_DEC_NW_TTL::type(), "DEC_NW_TTL", 0, 0},
    {AT_PUSH_PBB::type(), "PUSH_PBB", 0, 0},
    {AT_POP_PBB::type(), "POP_PBB", 0, 0},
    {deprecated::AT_ENQUEUE_V1::type(), "ENQUEUE", 0, 0},
    {ActionType{OFPAT_SET_FIELD, 0}, "SET_FIELD", 0, 0},
    {ActionType{OFPAT_EXPERIMENTER, 0}, "EXPERIMENTER", 0, 0}};

bool ActionType::parse(const std::string &s) {
  for (const auto &i : sActionInfo) {
    if (s == i.name) {
      value32_ = i.type.value32_;
      return true;
    }
  }
  return false;
}

const ActionTypeInfo *ActionType::lookupInfo() const {
  for (const auto &i : sActionInfo) {
    if (value32_ == i.type) {
      return &i;
    }
  }

  // Ignore length when checking for SET_FIELD or EXPERIMENTER actions.
  const UInt32 mask = BigEndianToNative(0xFFFF0000);
  UInt32 desiredValue = (value32_ & mask);
  for (unsigned i = ArrayLength(sActionInfo) - 2; i < ArrayLength(sActionInfo);
       ++i) {
    if (desiredValue == sActionInfo[i].type) {
      return &sActionInfo[i];
    }
  }

  return nullptr;
}

const ActionTypeInfo *ActionType::lookupInfo_IgnoreLength() const {
  const UInt32 mask = BigEndianToNative(0xFFFF0000);
  UInt32 desiredValue = (value32_ & mask);
  for (const auto &i : sActionInfo) {
    if (desiredValue == (i.type & mask)) {
      return &i;
    }
  }

  return nullptr;
}
