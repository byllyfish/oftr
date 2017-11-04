// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "./oftr_help.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include "ofp/oxmtype.h"
#include "ofp/rpc/rpcevents.h"
#include "ofp/yaml/ybundleaddmessage.h"
#include "ofp/yaml/ybundlecontrol.h"
#include "ofp/yaml/yecho.h"
#include "ofp/yaml/yerror.h"
#include "ofp/yaml/yfeaturesreply.h"
#include "ofp/yaml/yflowmod.h"
#include "ofp/yaml/yflowremoved.h"
#include "ofp/yaml/ygetasyncreply.h"
#include "ofp/yaml/ygetconfigreply.h"
#include "ofp/yaml/ygroupmod.h"
#include "ofp/yaml/yheaderonly.h"
#include "ofp/yaml/yhello.h"
#include "ofp/yaml/ymetermod.h"
#include "ofp/yaml/ymultipartreply.h"
#include "ofp/yaml/ymultipartrequest.h"
#include "ofp/yaml/ypacketin.h"
#include "ofp/yaml/ypacketout.h"
#include "ofp/yaml/yportmod.h"
#include "ofp/yaml/yportstatus.h"
#include "ofp/yaml/yqueuegetconfigreply.h"
#include "ofp/yaml/yqueuegetconfigrequest.h"
#include "ofp/yaml/yrawmessage.h"
#include "ofp/yaml/yrequestforward.h"
#include "ofp/yaml/yrolereply.h"
#include "ofp/yaml/yrolerequest.h"
#include "ofp/yaml/yrolestatus.h"
#include "ofp/yaml/yschema.h"
#include "ofp/yaml/ysetasync.h"
#include "ofp/yaml/ysetconfig.h"
#include "ofp/yaml/ytablemod.h"
#include "ofp/yaml/ytablestatus.h"

inline std::ostream &operator<<(std::ostream &os, llvm::StringRef s) {
  return os.write(s.data(), ofp::Signed_cast(s.size()));
}

using namespace ofpx;

// Declare separate constructor/destructor to compile `schemas_`.
Help::Help() = default;
Help::~Help() = default;

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

static const char *const kMessageSchemas[] = {
    llvm::yaml::kHelloSchema,
    llvm::yaml::kErrorSchema,
    llvm::yaml::kEchoRequestSchema,
    llvm::yaml::kEchoReplySchema,
    llvm::yaml::kFeaturesRequestSchema,
    llvm::yaml::kFeaturesReplySchema,
    llvm::yaml::kGetConfigRequestSchema,
    llvm::yaml::kGetConfigReplySchema,
    llvm::yaml::kSetConfigSchema,
    llvm::yaml::kPacketInSchema,
    llvm::yaml::kFlowRemovedSchema,
    llvm::yaml::kPortStatusSchema,
    llvm::yaml::kPacketOutSchema,
    llvm::yaml::kFlowModSchema,
    llvm::yaml::kGroupModSchema,
    llvm::yaml::kPortModSchema,
    llvm::yaml::kTableModSchema,
    llvm::yaml::kMultipartRequestSchema,
    llvm::yaml::kMultipartReplySchema,
    llvm::yaml::kBarrierRequestSchema,
    llvm::yaml::kBarrierReplySchema,
    llvm::yaml::kQueueGetConfigRequestSchema,
    llvm::yaml::kQueueGetConfigReplySchema,
    llvm::yaml::kRoleRequestSchema,
    llvm::yaml::kRoleReplySchema,
    llvm::yaml::kGetAsyncRequestSchema,
    llvm::yaml::kGetAsyncReplySchema,
    llvm::yaml::kSetAsyncSchema,
    llvm::yaml::kMeterModSchema,
    llvm::yaml::kRoleStatusSchema,
    llvm::yaml::kTableStatusSchema,
    llvm::yaml::kRequestForwardSchema,
    llvm::yaml::kBundleControlSchema,
    llvm::yaml::kBundleAddMessageSchema,
    llvm::yaml::kRawMessageSchema,
};

static const char *const kInstructionSchemas[] = {
    llvm::yaml::kGotoTableSchema,
    llvm::yaml::kWriteMetadataSchema,
    llvm::yaml::kWriteActionsSchema,
    llvm::yaml::kApplyActionsSchema,
    llvm::yaml::kClearActionsSchema,
    llvm::yaml::kMeterSchema,
    llvm::yaml::kExperimenterInstructionSchema,
};

static const char *const kActionSchemas[] = {
    llvm::yaml::kCopyTTLOutSchema,
    llvm::yaml::kCopyTTLInSchema,
    llvm::yaml::kDecMPLSTTLSchema,
    llvm::yaml::kPopVLANSchema,
    llvm::yaml::kDecNwTTLSchema,
    llvm::yaml::kPopPBBSchema,
    llvm::yaml::kOutputSchema,
    llvm::yaml::kSetMPLSTTLSchema,
    llvm::yaml::kPushVLANSchema,
    llvm::yaml::kPushMPLSSchema,
    llvm::yaml::kPopMPLSSchema,
    llvm::yaml::kSetQueueSchema,
    llvm::yaml::kGroupSchema,
    llvm::yaml::kSetNwTTLSchema,
    llvm::yaml::kPushPBBSchema,
    llvm::yaml::kEnqueueSchema,
    llvm::yaml::kSetFieldSchema,
    llvm::yaml::kExperimenterActionSchema,
    llvm::yaml::kNiciraRegMoveActionSchema,
    llvm::yaml::kNiciraRegLoadActionSchema};

static const char *const kMeterBandSchemas[] = {
    llvm::yaml::kMeterBandDropSchema, llvm::yaml::kMeterBandDscpRemarkSchema,
    llvm::yaml::kMeterBandExperimenterSchema,
};

static const char *const kStructSchemas[] = {
    llvm::yaml::kBucketSchema,        llvm::yaml::kPortSchema,
    llvm::yaml::kQueueSchema,         llvm::yaml::kTableDescSchema,
    llvm::yaml::kPacketCounterSchema,
};

static const char *const kPropertySchemas[] = {
    llvm::yaml::kExperimenterPropertySchema,
    llvm::yaml::kAsyncConfigExperimenterPropertySchema,
};

static const char *const kBuiltinTypes[] = {
    "UInt8",         "UInt16",      "UInt32",       "UInt64",
    "UInt24",        "SInt32",      "String",       "Str16",
    "Str32",         "Str256",      "HexData",      "DatapathID",
    "MacAddress",    "IPv4Address", "IPv6Address",  "IPEndpoint",
    "LLDPChassisID", "LLDPPortID",  "ActionID",     "FieldID",
    "InstructionID", "Timestamp",   "RegisterBits", "DurationSec",
    "VlanNumber"};

using SchemaPair = std::pair<ofp::yaml::SchemaMakerFunction, const char *>;

static const SchemaPair kEnumSchemas[] = {
    {ofp::yaml::MakeSchema<ofp::OFPFlowModCommand>, "Enum/FlowModCommand"},
    {ofp::yaml::MakeSchema<ofp::OFPPacketInReason>, "Enum/PacketInReason"},
    {ofp::yaml::MakeSchema<ofp::OFPPortStatusReason>, "Enum/PortStatusReason"},
    {ofp::yaml::MakeSchema<ofp::OFPFlowRemovedReason>,
     "Enum/FlowRemovedReason"},
    {ofp::yaml::MakeSchema<ofp::OFPTableStatusReason>,
     "Enum/TableStatusReason"},
    {ofp::yaml::MakeSchema<ofp::OFPControllerRole>, "Enum/ControllerRole"},
    {ofp::yaml::MakeSchema<ofp::OFPMeterModCommand>, "Enum/MeterModCommand"},
    {ofp::yaml::MakeSchema<ofp::OFPErrorType>, "Enum/ErrorType"},
    {ofp::yaml::MakeSchema<ofp::OFPFlowUpdateEvent>, "Enum/FlowUpdateEvent"},
    {ofp::yaml::MakeSchema<ofp::OFPErrorCode>, "Enum/ErrorCode"},
    {ofp::yaml::MakeSchema<ofp::OFPGroupModCommand>, "Enum/GroupModCommand"},
    {ofp::yaml::MakeSchema<ofp::OFPGroupType>, "Enum/GroupType"},
    {ofp::yaml::MakeSchema<ofp::OFPFlowMonitorCommand>,
     "Enum/FlowMonitorCommand"},
    {ofp::yaml::MakeSchema<ofp::OFPRoleStatusReason>, "Enum/RoleStatusReason"},
    {ofp::yaml::MakeSchema<ofp::OFPBundleCtrlType>, "Enum/BundleCtrlType"},
};

static const SchemaPair kMixedSchemas[] = {
    {ofp::yaml::MakeSchema<ofp::PortNumber>, "Mixed/PortNumber"},
    {ofp::yaml::MakeSchema<ofp::QueueNumber>, "Mixed/QueueNumber"},
    {ofp::yaml::MakeSchema<ofp::GroupNumber>, "Mixed/GroupNumber"},
    {ofp::yaml::MakeSchema<ofp::BufferNumber>, "Mixed/BufferNumber"},
    {ofp::yaml::MakeSchema<ofp::TableNumber>, "Mixed/TableNumber"},
    {ofp::yaml::MakeSchema<ofp::MeterNumber>, "Mixed/MeterNumber"},
    {ofp::yaml::MakeSchema<ofp::ControllerMaxLen>, "Mixed/ControllerMaxLen"},
};

static const SchemaPair kFlagSchemas[] = {
    {ofp::yaml::MakeFlagSchema<ofp::OFPFlowModFlags>, "Flag/FlowModFlags"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPActionTypeFlags>,
     "Flag/ActionTypeFlags"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPCapabilitiesFlags>,
     "Flag/CapabilitiesFlags"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPConfigFlags>,
     "Flag/ConfigFlags=FRAG_NORMAL,FRAG_DROP,FRAG_REASM"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPPortFeaturesFlags>,
     "Flag/PortFeaturesFlags"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPOpticalPortFeaturesFlags>,
     "Flag/OpticalPortFeaturesFlags"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPPortConfigFlags>,
     "Flag/PortConfigFlags"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPPortStateFlags>,
     "Flag/PortStateFlags=STP_LISTEN,STP_LEARN,STP_FORWARD"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPMultipartFlags>, "Flag/MultipartFlags"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPMeterConfigFlags>,
     "Flag/MeterConfigFlags"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPPacketInFlags>, "Flag/PacketInFlags"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPPortStatusFlags>,
     "Flag/PortStatusFlags"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPFlowRemovedFlags>,
     "Flag/FlowRemovedFlags"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPRoleStatusFlags>,
     "Flag/RoleStatusFlags"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPTableStatusFlags>,
     "Flag/TableStatusFlags"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPTableConfigFlags>,
     "Flag/TableConfigFlags"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPRequestForwardFlags>,
     "Flag/RequestForwardFlags"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPFlowMonitorFlags>,
     "Flag/FlowMonitorFlags"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPBundleFlags>, "Flag/BundleFlags"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPMeterBandFlags>, "Flag/MeterBandFlags"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPGroupCapabilityFlags>,
     "Flag/GroupCapabilityFlags"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPGroupTypeFlags>, "Flag/GroupTypeFlags"},
    {ofp::yaml::MakeFlagSchema<ofp::OFPMeterFlags>, "Flag/MeterFlags"},
};

// Translate "BigNN" to "UIntNN" for documentation purposes.
static const std::pair<const char *, const char *> kFieldTypeMap[] = {
    {"Big8", "UInt8"},
    {"Big16", "UInt16"},
    {"Big32", "UInt32"},
    {"Big64", "UInt64"},
    {"Big24", "UInt24"},
    {"LLDPValue<LLDPType::ChassisID>", "LLDPChassisID"},
    {"LLDPValue<LLDPType::PortID>", "LLDPPortID"}};

OFP_END_IGNORE_GLOBAL_CONSTRUCTOR

int Help::run(int argc, const char *const *argv) {
  parseCommandLineOptions(
      argc, argv, "Provide information about the OpenFlow YAML schema\n");
  loadSchemas();

  if (fieldTable_) {
    dumpFieldTable();
  } else if (messages_) {
    listSchemas("Message");
  } else if (instructions_) {
    listSchemas("Instruction");
  } else if (actions_) {
    listSchemas("Action");
  } else if (builtins_) {
    listSchemas("Builtin");
  } else if (fields_) {
    listSchemas("Field");
  } else if (enums_) {
    listSchemas("Enum");
  } else if (flags_) {
    listSchemas("Flag");
  } else if (mixed_) {
    listSchemas("Mixed");
  } else if (rpc_) {
    listSchemas("Rpc");
  } else if (schemaAll_) {
    dumpSchemaAll();
  } else if (schemaNames_) {
    dumpSchemaNames();
  } else if (schemaLexicon_) {
    dumpSchemaLexicon();
  } else if (!args_.empty()) {
    for (auto &arg : args_) {
      printSchema(arg);
    }
  } else {
    cl::PrintHelpMessage(false, true);
    return 1;
  }

  return 0;
}

void Help::loadSchemas() {
  loadSchema(llvm::yaml::kRpcSchema);

  for (auto &schema : kMessageSchemas) {
    loadSchema(schema);
  }

  for (auto &schema : kInstructionSchemas) {
    loadSchema(schema);
  }

  for (auto &schema : kActionSchemas) {
    loadSchema(schema);
  }

  for (auto &schema : kMeterBandSchemas) {
    loadSchema(schema);
  }

  for (auto &schema : kStructSchemas) {
    loadSchema(schema);
  }

  for (auto &schema : kPropertySchemas) {
    loadSchema(schema);
  }

  for (auto &p : kEnumSchemas) {
    loadSchema(p.first(p.second));
  }

  for (auto &p : kMixedSchemas) {
    loadSchema(p.first(p.second));
  }

  for (auto &p : kFlagSchemas) {
    loadSchema(p.first(p.second));
  }

  addFieldSchemas();
  addBuiltinTypes();
  initTopLevel();
}

void Help::loadSchema(const std::string &schema) {
  assert(!schema.empty());
  // If the first line in the schema is empty, split it on the empty lines
  // into multiple schema objects.
  if (schema[0] == '\n') {
    llvm::SmallVector<llvm::StringRef, 25> vals;
    llvm::StringRef{schema}.drop_front(1).split(vals, "\n\n", -1, false);
    for (auto val : vals) {
      schemas_.emplace_back(new Schema{val});
    }

  } else {
    schemas_.emplace_back(new Schema{schema});
  }
}

void Help::addFieldSchemas() {
  for (size_t i = 0; i < ofp::OXMTypeInfoArraySize; ++i) {
    const ofp::OXMTypeInfo *info = &ofp::OXMTypeInfoArray[i];

    std::string buf;
    llvm::raw_string_ostream oss{buf};
    oss << "{Field/" << info->name << "}\n";
    oss << "field: " << info->name << '\n';
    oss << "value: " << translateFieldType(info->type) << '\n';
    oss << "mask: !optout " << translateFieldType(info->type) << '\n';

    loadSchema(oss.str());
  }
}

void Help::addBuiltinTypes() {
  const std::string builtin{"Builtin/"};
  for (auto &p : kBuiltinTypes) {
    loadSchema(builtin + p + "\n<builtin>");
  }
}

/// Build list of the names of the top level types in the schema list.
void Help::initTopLevel() {
  for (auto &s : schemas_) {
    topLevel_.insert(s->type().lower());
  }
}

Schema *Help::findSchema(const std::string &key) {
  auto iter = std::find_if(
      schemas_.begin(), schemas_.end(),
      [&key](std::unique_ptr<Schema> &schema) { return schema->equals(key); });

  return iter != schemas_.end() ? iter->get() : nullptr;
}

Schema *Help::findNearestSchema(const std::string &key) {
  std::string s = llvm::StringRef{key}.upper();

  unsigned minDistance = 0xffff;
  Schema *minSchema = nullptr;

  for (auto &schema : schemas_) {
    auto buf = schema->name().upper();
    auto name = llvm::StringRef{buf};
    if (name.startswith(s))
      return schema.get();
    unsigned dist = name.edit_distance(s, true, minDistance);
    if (dist < minDistance) {
      minDistance = dist;
      minSchema = schema.get();
    }
  }

  return minSchema;
}

void Help::dumpFieldTable() {
  // Determine the maximum width of the name and type fields.
  int nameWidth = 0;
  int typeWidth = 0;

  for (size_t i = 0; i < ofp::OXMTypeInfoArraySize; ++i) {
    const ofp::OXMTypeInfo *info = &ofp::OXMTypeInfoArray[i];
    auto typeStr = translateFieldType(info->type);
    nameWidth = std::max(nameWidth, static_cast<int>(std::strlen(info->name)));
    typeWidth = std::max(typeWidth, static_cast<int>(std::strlen(typeStr)));
  }

  // Print the header line.
  std::cout << std::setw(nameWidth) << std::left << "Name"
            << " | " << std::setw(typeWidth) << std::left << "Type"
            << " | " << std::setw(21) << "OXM ID" 
            << " | "
            << "Description\n";
  std::cout << std::setfill('-');
  std::cout << std::setw(nameWidth) << "-"
            << "-|-" << std::setw(typeWidth) << "-"
            << "-|-" << std::setw(21) << "-"
            << "-|-" << std::setw(12) << "-" << '\n';
  std::cout << std::setfill(' ');

  // Print the name, type and description of each field.
  for (size_t i = 0; i < ofp::OXMTypeInfoArraySize; ++i) {
    const ofp::OXMTypeInfo *info = &ofp::OXMTypeInfoArray[i];
    auto typeStr = translateFieldType(info->type);
    auto oxmStr = translateFieldNum(info->value32, info->experimenter);
    std::cout << std::setw(nameWidth) << std::left << info->name << " | "
              << std::setw(typeWidth) << std::left << typeStr << " | "
              << std::setw(21) << oxmStr << " | "
              << info->description << '\n';
  }
}

void Help::listSchemas(const std::string &type) {
  // Print out the name of each schema with the specified type.
  for (auto &schema : schemas_) {
    if (schema->type() == type) {
      std::cout << schema->name() << '\n';
    }
  }
}

void Help::printSchema(const std::string &key) {
  // If key is a top level type, we print out all sub-schemas, but no
  // dependent types.
  std::string lowerKey = llvm::StringRef{key}.lower();
  if (topLevel_.find(lowerKey) != topLevel_.end()) {
    for (auto &s : schemas_) {
      if (s->type().equals_lower(lowerKey))
        s->print(std::cout, brief_);
    }
    return;
  }

  // Otherwise, we search for the first schema name that matches.
  auto schema = findSchema(key);
  if (schema) {
    schema->print(std::cout, brief_);

    for (auto &s : schema->dependsOnSchemas()) {
      auto depSchema = findSchema(s);
      if (depSchema) {
        depSchema->print(std::cout, brief_);
      } else {
        llvm::errs() << "Unknown dependent schema '" << s << "'\n";
      }
    }

  } else {
    schema = findNearestSchema(key);
    if (schema) {
      llvm::errs() << "Unknown command line argument '" << key
                   << "'. Did you mean '" << schema->name() << "'?" << '\n';
    }
  }
}

/// Print out the type/name for each schema.
void Help::dumpSchemaNames() {
  for (auto &schema : schemas_) {
    std::cout << schema->type().lower() << '/' << schema->name() << '\n';
  }
}

/// Print out each schema. Check that dependent schema's exist.
void Help::dumpSchemaAll() {
  bool missingDependentSchemas = false;

  for (auto &schema : schemas_) {
    schema->print(std::cout, brief_);

    for (auto &s : schema->dependsOnSchemas()) {
      if (!findSchema(s)) {
        llvm::errs() << "Unknown dependent schema '" << s << "'\n";
        missingDependentSchemas = true;
      }
    }
  }

  if (missingDependentSchemas) {
    llvm::errs() << "Some dependent schemas are missing.\n";
    ::exit(static_cast<int>(ExitStatus::MissingDependentSchemas));
  }
}

void Help::dumpSchemaLexicon() {
  std::set<std::string> lexicon;

  for (auto &schema : schemas_) {
    std::stringstream ss;
    schema->print(ss, false);

    std::string line;
    while (std::getline(ss, line)) {
      llvm::StringRef s{line};
      s = s.trim();
      if (s.empty())
        continue;
      char ch = s.front();
      if ((ch == '_') || (ch >= 'a' && ch <= 'z')) {
        auto p = s.split(':');
        if (!p.second.empty()) {
          lexicon.insert(p.first.str());
        }
      }
    }
  }

  for (auto word : lexicon) {
    std::cout << word << '\n';
  }
}

const char *Help::translateFieldType(const char *type) {
  for (auto &entry : kFieldTypeMap) {
    if (std::strcmp(type, entry.first) == 0) {
      return entry.second;
    }
  }
  return type;
}

std::string Help::translateFieldNum(ofp::UInt32 value32, ofp::UInt32 experimenter) {
  std::string buf;
  llvm::raw_string_ostream os{buf};
  value32 = ofp::detail::HostSwapByteOrder(value32);
  if (experimenter) {
    os << llvm::format("0x%0.8x.0x%0.8x", value32, experimenter);
  } else {
    os << llvm::format("0x%0.8x           ", value32);
  }
  return os.str();
}
