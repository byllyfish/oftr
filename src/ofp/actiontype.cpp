#include "ofp/actiontype.h"
#include "ofp/actions.h"

using namespace ofp;

static const ActionTypeInfo sActionInfo[] = {
    {AT_OUTPUT::type(), "OFPAT_OUTPUT"},
    {AT_COPY_TTL_OUT::type(), "OFPAT_COPY_TTL_OUT"},
    {AT_COPY_TTL_IN::type(), "OFPAT_COPY_TTL_IN"},
    {AT_SET_MPLS_TTL::type(), "OFPAT_SET_MPLS_TTL"},
    {AT_DEC_MPLS_TTL::type(), "OFPAT_DEC_MPLS_TTL"},
    {AT_PUSH_VLAN::type(), "OFPAT_PUSH_VLAN"},
    {AT_POP_VLAN::type(), "OFPAT_POP_VLAN"},
    {AT_PUSH_MPLS::type(), "OFPAT_PUSH_MPLS"},
    {AT_POP_MPLS::type(), "OFPAT_POP_MPLS"},
    {AT_SET_QUEUE::type(), "OFPAT_SET_QUEUE"},
    {AT_GROUP::type(), "OFPAT_GROUP"},
    {AT_SET_NW_TTL::type(), "OFPAT_SET_NW_TTL"},
    {AT_DEC_NW_TTL::type(), "OFPAT_DEC_NW_TTL"},
    {AT_PUSH_PBB::type(), "OFPAT_PUSH_PBB"},
    {AT_POP_PBB::type(), "OFPAT_POP_PBB"},
    {ActionType{OFPAT_SET_FIELD, 0}, "OFPAT_SET_FIELD"},
    {ActionType{OFPAT_EXPERIMENTER, 0}, "OFPAT_EXPERIMENTER"}};

bool ActionType::parse(const std::string &s) {
  for (unsigned i = 0; i < ArrayLength(sActionInfo); ++i) {
    if (s == sActionInfo[i].name) {
      value32_ = sActionInfo[i].type.value32_;
      return true;
    }
  }
  return false;
}

const ActionTypeInfo *ActionType::lookupInfo() const {
  const UInt32 mask = BigEndianToNative(0xFFFF0000);
  UInt32 desiredValue = (value32_ & mask);
  for (unsigned i = 0; i < ArrayLength(sActionInfo); ++i) {
    if (desiredValue == (sActionInfo[i].type & mask)) {
      return &sActionInfo[i];
    }
  }

  return nullptr;
}
