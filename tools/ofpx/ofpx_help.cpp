// Copyright 2014-present Bill Fisher. All rights reserved.

#include "./ofpx_help.h"
#include <iostream>
#include <iomanip>
#include "ofp/oxmtype.h"

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
    auto typeStr = translateFieldType(info->type);
    nameWidth = std::max(nameWidth, static_cast<int>(std::strlen(info->name)));
    typeWidth = std::max(typeWidth, static_cast<int>(std::strlen(typeStr)));
  }

  // Print out the name, type and description of each field.

  for (size_t i = 0; i < ofp::OXMTypeInfoArraySize; ++i) {
    const ofp::OXMTypeInfo *info = &ofp::OXMTypeInfoArray[i];
    auto typeStr = translateFieldType(info->type);
    std::cout << std::setw(nameWidth) << std::left << info->name << " | "
              << std::setw(typeWidth) << std::left << typeStr << " | "
              << info->description << '\n';
  }
}

// Translate "BigNN" to "UIntNN" for documentation purposes.

static std::pair<const char *, const char *> sFieldTypeMap[] = {
  { "Big8", "UInt8" },
  { "Big16", "UInt16" },
  { "Big32", "UInt32" },
  { "Big64", "UInt64" }
};

const char *Help::translateFieldType(const char *type) {
  for (auto &entry: sFieldTypeMap) {
    if (std::strcmp(type, entry.first) == 0) {
      return entry.second;
    }
  }
  return type;
}
