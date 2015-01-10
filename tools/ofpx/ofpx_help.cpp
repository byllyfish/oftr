#include "ofpx_help.h"
#include "ofp/oxmtype.h"
#include <iostream>

using namespace ofpx;

int Help::run(int argc, const char *const *argv) {
  listFields();

  return 0;
}


void Help::listFields() {
  for (size_t i = 0; i < ofp::OXMTypeInfoArraySize; ++i) {
    const ofp::OXMTypeInfo *info = &ofp::OXMTypeInfoArray[i];
    std::cout << info->name << '\n';
  }
}
