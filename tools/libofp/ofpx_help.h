// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef TOOLS_LIBOFP_OFPX_HELP_H_
#define TOOLS_LIBOFP_OFPX_HELP_H_

#include "./ofpx.h"
#include <set>

namespace ofp {
namespace yaml {

class Schema;

}  // namespace yaml
}  // namespace ofp

namespace ofpx {

using ofp::yaml::Schema;

// libofp help [--fields|-f] [--actions|-a] [--instructions|-i] [--messages|-m]
// [args...]
//
// Access built-in help features.
//
//   --fields         List supported match fields.
//   --actions        List supported actions.
//   --builtins       List supported builtin types.
//   --instructions   List supported instructions.
//   --messages       List supported message types.
//   --enums          List supported enumeration types.
//   --flags          List supported flags.
//   --mixed          List supported mixed types.
//   --rpc            List supported RPC commands.

class Help : public Subprogram {
 public:
  enum class ExitStatus {
    Success = 0,
    MissingDependentSchemas = MinExitStatus
  };

  int run(int argc, const char *const *argv) override;

  // Declare separate constructor/destructor to compile `schemas_`.
  Help();
  ~Help();

 private:
  std::vector<std::unique_ptr<Schema>> schemas_;
  std::set<std::string> topLevel_;

  void loadSchemas();
  void loadSchema(const std::string &schema);
  void addFieldSchemas();
  void addBuiltinTypes();
  void initTopLevel();

  Schema *findSchema(const std::string &key);
  Schema *findNearestSchema(const std::string &key);

  void dumpFieldTable();
  void listSchemas(const std::string &type);
  void printSchema(const std::string &key);

  void dumpSchemaNames();
  void dumpSchemaAll();

  static const char *translateFieldType(const char *type);

  // --- Command-line Arguments ---
  cl::opt<bool> fields_{"fields", cl::desc("List supported match fields.")};
  cl::opt<bool> fieldTable_{
      "field-table", cl::desc("List supported match fields in a table.")};
  cl::opt<bool> actions_{"actions", cl::desc("List supported actions.")};
  cl::opt<bool> builtins_{"builtins",
                          cl::desc("List supported builtin types.")};
  cl::opt<bool> instructions_{"instructions",
                              cl::desc("List supported instructions.")};
  cl::opt<bool> messages_{"messages", cl::desc("List supported messages.")};
  cl::opt<bool> enums_{"enums", cl::desc("List supported enumerated types.")};
  cl::opt<bool> flags_{"flags", cl::desc("List supported flag types.")};
  cl::opt<bool> mixed_{"mixed", cl::desc("List supported mixed types.")};
  cl::opt<bool> rpc_{"rpc", cl::desc("List supported RPC commands.")};
  cl::opt<bool> schemaNames_{"schema-names",
                             cl::desc("List all schema names.")};
  cl::opt<bool> schemaAll_{"schema-all", cl::desc("List complete schema.")};
  cl::opt<bool> brief_{"brief",
                       cl::desc("Display abbreviated form of message schema")};
  cl::list<std::string> args_{cl::Positional, cl::desc("<Args>")};

  // --- Argument Aliases ---
  cl::alias fAlias_{"f", cl::desc("Alias for -fields"), cl::aliasopt(fields_)};
  cl::alias aAlias_{"a", cl::desc("Alias for -actions"),
                    cl::aliasopt(actions_)};
  cl::alias iAlias_{"i", cl::desc("Alias for -instructions"),
                    cl::aliasopt(instructions_)};
  cl::alias mAlias_{"m", cl::desc("Alias for -messages"),
                    cl::aliasopt(messages_)};
  cl::alias rAlias_{"r", cl::desc("Alias for -rpc"), cl::aliasopt(rpc_)};
  cl::alias bAlias_{"b", cl::desc("Alias for -brief"), cl::aliasopt(brief_)};
};

}  // namespace ofpx

#endif  // TOOLS_LIBOFP_OFPX_HELP_H_
