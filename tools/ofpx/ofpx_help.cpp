#include "ofpx_help.h"
#include "ofp/oxmtype.h"
#include <iostream>
#include <iomanip>

using namespace ofpx;

int Help::run(int argc, const char *const *argv) {
  listFields();

  return 0;
}


void Help::listFields() {
  // Determine the maximum width of the name and type fields.
  
  int nameWidth = 0;
  int typeWidth = 0;

  for (size_t i = 0; i < ofp::OXMTypeInfoArraySize; ++i) {
    const ofp::OXMTypeInfo *info = &ofp::OXMTypeInfoArray[i];
    nameWidth = std::max(nameWidth, static_cast<int>(std::strlen(info->name)));
    typeWidth = std::max(typeWidth, static_cast<int>(std::strlen(info->type)));
  }

  // Print out the name, type and description of each field.

  for (size_t i = 0; i < ofp::OXMTypeInfoArraySize; ++i) {
    const ofp::OXMTypeInfo *info = &ofp::OXMTypeInfoArray[i];
    std::cout << std::setw(nameWidth) << std::left << info->name << " | " << std::setw(typeWidth) << std::left << info->type << " | " << info->description << '\n';
  }
}
