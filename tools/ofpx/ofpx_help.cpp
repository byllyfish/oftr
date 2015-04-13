// Copyright 2014-present Bill Fisher. All rights reserved.

#include "./ofpx_help.h"
#include <iostream>
#include <iomanip>
#include "ofp/oxmtype.h"
#include "ofp/yaml/yhello.h"
#include "ofp/yaml/yerror.h"
#include "ofp/yaml/yecho.h"
#include "ofp/yaml/yfeaturesreply.h"
#include "ofp/yaml/ysetconfig.h"
#include "ofp/yaml/ypacketin.h"
#include "ofp/yaml/ypacketout.h"
#include "ofp/yaml/yflowmod.h"
#include "ofp/yaml/yflowremoved.h"
#include "ofp/yaml/yportmod.h"
#include "ofp/yaml/yschema.h"

using namespace ofpx;

// Declare separate constructor/destructor to compile `schemas_`.
Help::Help() = default;
Help::~Help() = default;

static const char *const kMessageSchemas[] = {
  llvm::yaml::kHelloSchema,
  llvm::yaml::kErrorSchema,
  llvm::yaml::kEchoRequestSchema,
  llvm::yaml::kEchoReplySchema,
  llvm::yaml::kFeaturesReplySchema,
  llvm::yaml::kSetConfigSchema,
  llvm::yaml::kPortModSchema,
  llvm::yaml::kPacketOutSchema,
  llvm::yaml::kPacketInSchema,
  llvm::yaml::kFlowModSchema,
  llvm::yaml::kFlowRemovedSchema
};

int Help::run(int argc, const char *const *argv) {
  cl::ParseCommandLineOptions(argc, argv);
  loadSchemas();

  if (fields_) {
    listFields();
  } else if (messages_) {
    listMessages();
  } else {
    for (auto &arg : args_) {
      printArg(arg);
    }
  }

  return 0;
}

void Help::loadSchemas() {
  for (auto &schema : kMessageSchemas) {
    schemas_.emplace_back(new Schema{schema});
  }
}

Schema *Help::findSchema(const std::string &key) {
  auto iter = std::find_if(schemas_.begin(), schemas_.end(), [&key](auto &schema){
    return schema->equals(key);
  });

  return iter != schemas_.end() ? iter->get() : nullptr;
}

Schema *Help::findNearestSchema(const std::string &key) {
  std::string s = llvm::StringRef{key}.upper();

  unsigned minDistance = 0xffff;
  Schema *minSchema = nullptr;

  for (auto &schema : schemas_) {
    if (schema->name().startswith(s))
      return schema.get();
    unsigned dist = schema->name().edit_distance(s, true, minDistance);
    if (dist < minDistance) {
      minDistance = dist;
      minSchema = schema.get();
    }
  }

  return minSchema;
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

void Help::listMessages() {
  // Print out the name of each 'Message' schema.
  
  for (auto &schema : schemas_) {
    if (schema->type() == "Message") {
      std::cout << schema->name() << '\n';
    }
  }
}

void Help::printArg(const std::string &arg) {
  auto schema = findSchema(arg);
  if (schema) {
    std::cout << schema->value() << '\n';
  } else {
    schema = findNearestSchema(arg);
    if (schema) {
      std::cerr << "Unknown command line argument '" << arg << "'. Did you mean '" << schema->name() << "'?" << '\n';
    }
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
