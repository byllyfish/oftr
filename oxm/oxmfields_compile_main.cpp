// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.
/// \file
/// \brief Program to produce source code for oxmfields_main.cpp and
/// oxmfields.h.
///
/// Without arguments, it generates source code for oxmfields_main.cpp.
/// With -h argument, it generates source code for oxmfields.h.

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "ofp/oxmtype.h"  // so we can define lookupInfo().

using namespace std;

static vector<string> Split(const string &str, char delimiter, bool trim);

static string SplitField(const string &str, size_t whichOne) {
  auto vals = Split(str, '.', true);
  if (whichOne >= vals.size())
    return "0";
  return vals[whichOne];
}

class OXMField {
 public:
  enum { NumFields = 8 };

  explicit OXMField(const vector<string> fields) : fields_{fields} {}

  const string &name() const { return fields_.at(0); }
  const string &clas() const { return fields_.at(1); }
  string field() const { return SplitField(fields_.at(2), 0); }
  string experimenter() const { return SplitField(fields_.at(2), 1); }
  const string &size() const { return fields_.at(3); }
  const string &valueType() const { return fields_.at(4); }
  const string &mask() const { return fields_.at(5); }

  const string &description() const { return fields_.at(7); }
  bool hasDescription() const {
    return fields_.size() >= 8 && !description().empty();
  }

  const string oxmType() const {
    std::stringstream ss;
    ss << "OXMType{" << clas() << "," << field() << "," << size() << "}";
    return ss.str();
  }

  const string &prereqStr() const { return fields_.at(6); }
  string &prereqStr() { return fields_.at(6); }
  bool hasPrereqs() const { return !prereqStr().empty(); }

  vector<string> prereqs() const { return Split(prereqStr(), ',', true); }

  void defineOXMValue(ostream &s) {
    if (hasDescription()) {
      s << "/// \\brief " << description() << '\n';
    }

    if (experimenter() == "0") {
      // If this is a regular (non-experimenter) field.
      s << "using " << name() << " = OXMValue<"
        << "OXMInternalID::" << name() << ", " << clas() << ", " << field();
    } else {
      // If this is an experimenter field.
      s << "using " << name() << " = OXMValueExperimenter<"
        << "OXMInternalID::" << name() << ", " << experimenter() << ", "
        << field();
    }

    s << ", " << valueType() << ", " << size() << ", " << mask();

    if (hasPrereqs()) {
      s << ", &OXMPrereq_" << name();
    }

    s << ">;\n\n";
  }

  void declareGlobalPrereq(ostream &s) {
    s << "extern const OXMRange OXMPrereq_" << name() << ";\n";
  }

  void defineOXMEnum(ostream &s) { s << "  " << name() << ",\n"; }

  void writeOXMVisitorCase(ostream &s) {
    s << "  case OXMInternalID::" << name() << ":\n";
    s << "    visitor->template visit<" << name() << ">();\n";
    s << "    break;\n";
  }

  void writePrereqCode(ostream &s) {
    s << "  {\n";
    s << "    OXMList list;\n";
    for (auto preq : prereqs()) {
      if (preq == "/")
        continue;
      if (preq.find('&') != string::npos) {
        auto vec = Split(preq, '&', true);
        if (vec.size() == 2) {
          s << "    list.add(" << vec[0] << "," << vec[1] << ");\n";
        } else {
          s << "/*\n";
          s << "$$$$$ Unexpected prereq: " << preq << '\n';
          s << "$$$$$ Aborting.\n";
          abort();
        }
      } else {
        s << "    list.add(" << preq << ");\n";
      }
    }
    s << "    WritePrereq(\"" << name() << "\", list.data(), list.size());\n";
    s << "  }\n\n";
  }

  void writeOXMTypeInfoCode(ostream &s) {
    s << "WriteOXMTypeInfo(" << oxmType() << ", " << experimenter() << ", "
      << mask() << ", \"" << name() << "\", ";
    if (hasPrereqs())
      s << "\"&ofp::OXMPrereq_" << name() << "\"";
    else
      s << "\"nullptr\"";
    s << ", \"" << valueType() << "\", \"" << description() << "\");\n";
  }

 private:
  vector<string> fields_;
};

static vector<OXMField> ReadInput(istream &stream);
static void WriteHeaderFile(ostream &stream, vector<OXMField> &fields);
static void WriteSourceFile(ostream &stream, vector<OXMField> &fields);
static void CompilePrereqs(vector<OXMField> &fields);

int main(int argc, char **argv) {
  vector<string> args{argv, argv + argc};

  bool header = (args.size() > 1) && args[1] == "-h";

  auto fields = ReadInput(cin);
  if (header) {
    WriteHeaderFile(cout, fields);
  } else {
    CompilePrereqs(fields);
    WriteSourceFile(cout, fields);
  }
}

static vector<OXMField> ReadInput(istream &stream) {
  vector<OXMField> records;

  int lineNum = 0;
  std::string line;
  while (std::getline(stream, line)) {
    ++lineNum;
    // Skip empty lines or lines beginning with '#'.
    if (line.empty() || line[0] == '#')
      continue;
    // Split line at each tab into a list of strings.
    auto fields = Split(line, '\t', true);
    if (fields.size() >= OXMField::NumFields) {
      // Skip header line
      if (fields[0] == "Name")
        continue;
      records.push_back(OXMField{fields});
    } else {
      // If there are not enough fields, log an error and skip the line.
      cerr << "Ignored:" << lineNum << ": " << line << '\n';
    }
  }

  return records;
}

static const char *HeaderFilePreamble =
    R"""(// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.
//
// !! DO NOT EDIT THIS FILE!!
//
// This file is automatically generated during the build
// process. Please make your changes to `ofp/oxmfields.tab`.

#ifndef OFP_OXMFIELDS_AUTOGENERATED_H  // NOLINT
#define OFP_OXMFIELDS_AUTOGENERATED_H

#include "ofp/durationsec.h"
#include "ofp/ipv4address.h"
#include "ofp/ipv6address.h"
#include "ofp/lldpvalue.h"
#include "ofp/macaddress.h"
#include "ofp/oxmrange.h"
#include "ofp/oxmvalue.h"
#include "ofp/portnumber.h"
#include "ofp/vlannumber.h"

)""";

static void WriteHeaderFile(ostream &stream, vector<OXMField> &fields) {
  stream << HeaderFilePreamble;
  stream << "namespace ofp {\n\n";

  for (auto field : fields) {
    if (field.hasPrereqs())
      field.declareGlobalPrereq(stream);
  }

  stream << "\n\n/// \\brief Defines an internal ID in the contiguous range "
            "0..(N-1).\n";
  stream << "enum class OXMInternalID : UInt16 {\n";
  for (auto field : fields) {
    field.defineOXMEnum(stream);
  }
  stream << "  UNKNOWN = 0xFFFFU\n";
  stream << "};\n\n";

  stream << "OFP_BEGIN_IGNORE_PADDING\n";
  stream << "  struct OXMTypeInternalMapEntry {\n";
  stream << "    UInt32 value32;\n";
  stream << "    Big32 experimenter;\n";
  stream << "    OXMInternalID id;\n";
  stream << "  };\n";
  stream << "OFP_END_IGNORE_PADDING\n\n";

  for (auto field : fields) {
    field.defineOXMValue(stream);
  }

  stream << "\nextern const OXMTypeInfo OXMTypeInfoArray[];\n";
  stream << "extern const size_t OXMTypeInfoArraySize;\n";
  stream
      << "extern const OXMTypeInternalMapEntry OXMTypeInternalMapArray[];\n\n";

  stream << "template <class Visitor>\n";
  stream << "void OXMDispatch(OXMInternalID id, Visitor *visitor) {\n";
  stream << "  switch (id) {\n";

  for (auto field : fields) {
    field.writeOXMVisitorCase(stream);
  }
  stream << "  case OXMInternalID::UNKNOWN: break;\n";
  stream << "  }\n";
  stream << "}\n";

  stream << '\n';
  stream << "}  // namespace ofp\n\n";
  stream << "#endif // OFP_OXMFIELDS_AUTOGENERATED_H  // NOLINT\n";
}

static const char *PrereqFunctionPreamble =
    R"""(// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.
//
// !! DO NOT EDIT THIS FILE!!
//
// This file is automatically generated during the build
// process. Please make your changes to `ofp/oxmfields.tab`.

#include <iostream>
#include "ofp/oxmfields.h"
#include "ofp/oxmlist.h"

using namespace ofp;

// We need to define this array to get the linker to compile this tool.
const ofp::OXMTypeInfo ofp::OXMTypeInfoArray[] = {
	{"none", nullptr, 0, 0, false, "none", "none"}
};
const size_t ofp::OXMTypeInfoArraySize = 0;

static void WritePrereq(const char *name, const UInt8 *data, size_t len)
{
  std::cout << "static const ofp::UInt8 data_" << name << "[" << len << "]={";
  for (size_t i = 0; i < len; ++i) {
    std::cout << unsigned(data[i]) << ",";
  }
  std::cout << "};\n";
  std::cout << "const ofp::OXMRange ofp::OXMPrereq_" << name << "{data_" << name << "," << len <<"};\n";
}


static void WriteOXMTypeInfo(OXMType type, UInt32 experimenter, bool maskSupported, const char *nameStr, const char *prereqs, const char *typeStr, const char *descriptionStr) {
  if (std::strncmp("OFB_", nameStr, 4) == 0 ||
      std::strncmp("OXS_", nameStr, 4) == 0 ||
      std::strncmp("NXM_", nameStr, 4) == 0 ||
      std::strncmp("ONF_", nameStr, 4) == 0) {
    nameStr += 4;
  }
  std::cout << "{ \"" << nameStr << "\", " << prereqs << ", " << type << ", " << experimenter << ", \"" << typeStr << "\", \"" << descriptionStr << "\"},\n";
}

int main() {
  std::cout << "#include \"ofp/oxmfields.h\"\n";
  
)""";

static void WriteSourceFile(ostream &stream, vector<OXMField> &fields) {
  stream << PrereqFunctionPreamble;

  for (auto field : fields) {
    if (field.hasPrereqs())
      field.writePrereqCode(stream);
  }

  stream
      << R"""(std::cout << "const ofp::OXMTypeInfo ofp::OXMTypeInfoArray[)""";
  stream << fields.size();
  stream << R"""(] = {\n";)""";

  for (auto field : fields) {
    field.writeOXMTypeInfoCode(stream);
  }

  stream << R"""(
    std::cout << "};\n";
  )""";

  stream << R"""(std::cout << "const size_t ofp::OXMTypeInfoArraySize=)""";
  stream << fields.size();
  stream << R"""(;\n";)""";

  stream << "}\n";
}

static string Identifier(const string &s) {
  auto first = s.find_first_of(" {[(&");
  return s.substr(0, first);
}

// Compile prerequisites.
static void CompilePrereqs(vector<OXMField> &fields) {
  unordered_map<string, const OXMField *> map;

  for (auto &field : fields) {
    map[field.name()] = &field;
  }

  for (auto &field : fields) {
    vector<string> preqs = field.prereqs();
    // If first preq is '/', do not process prereqs.
    if (!preqs.empty() && preqs[0] == "/")
      continue;
    // Find prereqStr for each preq and append it to preqresStr.
    unordered_set<string> processed;
    for (auto preq : preqs) {
      auto id = Identifier(preq);
      if (processed.find(id) == processed.end()) {
        processed.insert(id);
        auto pstr = map.find(id);
        if (pstr != map.end()) {
          if (pstr->second->hasPrereqs()) {
            field.prereqStr() =
                pstr->second->prereqStr() + ',' + field.prereqStr();
          }
        }
      }
    }
  }
}

static void Trim(string &s) {
  auto first = s.find_first_not_of(" \t\n\v\f\r");
  if (first == string::npos) {
    s.clear();
  } else {
    s.erase(0, first);
    s.erase(s.find_last_not_of(" \t\n\v\f\r") + 1);
  }
}

static vector<string> Split(const string &str, char delimiter, bool trim) {
  vector<string> result;
  istringstream iss{str};
  for (string f; getline(iss, f, delimiter);) {
    if (trim)
      Trim(f);
    result.push_back(f);
  }
  return result;
}

// We need to define this function to get the linker to compile this tool.
namespace ofp {
extern const OXMTypeInfo OXMTypeInfoArray[];
extern const size_t OXMTypeInfoArraySize;
}
const ofp::OXMTypeInfo ofp::OXMTypeInfoArray[] = {
    {"none", nullptr, 0, 0, "none", "none"}};
const size_t ofp::OXMTypeInfoArraySize = 0;
