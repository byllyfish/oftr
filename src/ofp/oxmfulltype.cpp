#include "ofp/oxmfulltype.h"

using namespace ofp;

bool OXMFullType::parse(const std::string &s) {
  // TODO(bfish): make faster
  for (size_t i = 0; i < OXMTypeInfoArraySize; ++i) {
    if (s == OXMTypeInfoArray[i].name) {
      id_ = static_cast<OXMInternalID>(i);
      type_.setValue32(OXMTypeInfoArray[i].value32);
      experimenter_ = OXMTypeInfoArray[i].experimenter;
      return true;
    }
  }

  return false;
}
