//  ===== ---- ofp/oxmfields_compile_main.cpp --------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Program to produce source code for oxmfields_main.cpp and oxmfields.h.
///
/// Without arguments, it generates source code for oxmfields_main.cpp.
/// With -h argument, it generates source code for oxmfields.h.
//  ===== ------------------------------------------------------------ =====  //

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include "ofp/oxmtype.h"			// so we can define lookupInfo().

using namespace std;

static vector<string> Split(const string &str, char delimiter, bool trim);

class OXMField {
public:
	enum { NumFields = 7 };

	explicit OXMField(const vector<string> fields) : fields_{fields} 
	{
	}
	
	const string &name() const { return fields_.at(0); }
	const string &clas() const { return fields_.at(1); }
	const string &field() const { return fields_.at(2); }
	const string &size() const { return fields_.at(3); }
	const string &valueType() const { return fields_.at(4); }
	const string &mask() const { return fields_.at(5); }

	const string &description() const { return fields_.at(7); }
	bool hasDescription() const { return fields_.size() >= 8 && !description().empty(); }

	const string oxmType() const { 
		std::stringstream ss;
		ss << "OXMType{" << clas() << "," << field() << "," << size() << "}";
		return ss.str();
	}

	const string &prereqStr() const { return fields_.at(6); }
	string &prereqStr() { return fields_.at(6); }
	bool hasPrereqs() const { return !prereqStr().empty(); }
	
	vector<string> prereqs() const 
	{
		return Split(prereqStr(), ',', true);
	}
	
	void defineOXMValue(ostream &s)
	{
		if (hasDescription()) {
			s << "/// \\brief " << description() << '\n';
		}

		s << "using " << name() << " = OXMValue<" << 
			"OXMInternalID::" << name() << ',' <<
			clas() << ',' <<
			field() << ',' <<
			valueType() << ',' <<
		    size() << ',' <<
			mask();
			
		if (hasPrereqs()) {
			s << ",&OXMPrereq_" << name();
		}
		
		s << ">;\n\n";
	}
	
	void declareGlobalPrereq(ostream &s)
	{
		s << "extern const OXMRange OXMPrereq_" << name() << ";\n";
	}

	void defineOXMEnum(ostream &s)
	{
		s << "  " << name() << ",\n";
	}

	void writeOXMVisitorCase(ostream &s)
	{
		s << "  case OXMInternalID::" << name() << ":\n";
		s << "    visitor->template visit<" << name() << ">();\n";
		s << "    break;\n";
	}
	
	void writePrereqCode(ostream &s)
	{
		s << "  {\n";
		s << "    OXMList list;\n";
		for (auto preq : prereqs()) {
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

	void writeOXMTypeInfoCode(ostream &s)
	{
		s << "WriteOXMTypeInfo(" << oxmType() << ", " << mask() << ", \"" << name() << "\", "; 
		if (hasPrereqs())
			s << "\"&ofp::OXMPrereq_" << name() << "\");\n";
		else 
			s << "\"nullptr\");\n";
	}
	
private:
	vector<string> fields_;
};


static vector<OXMField> ReadInput(istream &stream);
static void WriteHeaderFile(ostream &stream, vector<OXMField> &fields);
static void WriteSourceFile(ostream &stream, vector<OXMField> &fields);
static void CompilePrereqs(vector<OXMField> &fields);


int main(int argc, char **argv)
{
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


static vector<OXMField> ReadInput(istream &stream)
{
	vector<OXMField> records;
	
	int lineNum = 0;
	std::string line;
	while (std::getline(stream, line)) {
		if (++lineNum == 1)						// skip first line
			continue;
		auto fields = Split(line, '\t', true);
		if (fields.size() >= OXMField::NumFields) {
			records.push_back(OXMField{fields});
		} else {
			cerr << "Ignored:" << lineNum << ": " << line << '\n';
		}
	}
	
	return records;
}

static const char *HeaderFilePreamble = R"""(//
// !! DO NOT EDIT THIS FILE!!
//
// This file is automatically generated during the build
// process. Please make your changes to `ofp/oxmfields.tab`.

#ifndef OFP_OXMFIELDS_AUTOGENERATED_H
#define OFP_OXMFIELDS_AUTOGENERATED_H

#include "ofp/oxmrange.h"
#include "ofp/oxmvalue.h"
#include "ofp/enetaddress.h"
#include "ofp/ipv4address.h"
#include "ofp/ipv6address.h"

)""";

static void WriteHeaderFile(ostream &stream, vector<OXMField> &fields)
{
	stream << HeaderFilePreamble;
	stream << "namespace ofp {\n\n";
	
	for (auto field : fields) {
		if (field.hasPrereqs())
			field.declareGlobalPrereq(stream);
	}

	stream << "\n\n/// \\brief Defines an internal ID in the contiguous range 0..(N-1).\n";
	stream << "enum class OXMInternalID : UInt16 {\n";
	for (auto field: fields) {
		field.defineOXMEnum(stream);
	}
	stream << "  UNKNOWN = 0xFFFFU\n";
	stream << "};\n\n";
	
	stream << "OFP_BEGIN_IGNORE_PADDING\n";
	stream << "  struct OXMTypeInternalMapEntry {\n";
	stream << "    UInt32 value32;\n    OXMInternalID id;\n  };\n";
	stream << "OFP_END_IGNORE_PADDING\n\n";

	for (auto field : fields) {
		field.defineOXMValue(stream);
	}
	
	stream << "\nextern const OXMTypeInfo OXMTypeInfoArray[];\n";
	stream << "extern const size_t OXMTypeInfoArraySize;\n";
	stream << "extern const OXMTypeInternalMapEntry OXMTypeInternalMapArray[];\n\n";

	stream << "template <class Visitor>\n";
	stream << "void OXMDispatch(OXMInternalID id, Visitor *visitor) {\n";
	stream << "  switch (id) {\n";

	for (auto field : fields) {
		field.writeOXMVisitorCase(stream);
	}
	stream << "  case OXMInternalID::UNKNOWN: break;\n";
	stream << "  };\n";
	stream << "}\n";

	stream << '\n';
	stream << "} // namespace ofp\n\n";
	stream << "#endif //OFP_OXMFIELDS_AUTOGENERATED_H\n";
}


static const char *PrereqFunctionPreamble = R"""(//
// !! DO NOT EDIT THIS FILE!!
//
// This file is automatically generated during the build
// process. Please make your changes to `ofp/oxmfields.tab`.

#include "ofp/oxmfields.h"
#include "ofp/oxmlist.h"
#include <iostream>

using namespace ofp;

// We need to define this array to get the linker to compile this tool.
const ofp::OXMTypeInfo ofp::OXMTypeInfoArray[] = {
	{"none", nullptr, 0, false}
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


static void WriteOXMTypeInfo(OXMType type, bool maskSupported, const char *name, const char *prereqs) {
  std::cout << "{ \"" << name << "\", " << prereqs << ", " << type << ", " << maskSupported << "},\n";
}

int main() {
  std::cout << "#include \"ofp/oxmfields.h\"\n";
  
)""";


static void WriteSourceFile(ostream &stream, vector<OXMField> &fields)
{	
	stream << PrereqFunctionPreamble;
	
	for (auto field : fields) {
		if (field.hasPrereqs())
			field.writePrereqCode(stream);
	}

	stream << R"""(std::cout << "const ofp::OXMTypeInfo ofp::OXMTypeInfoArray[)""";
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


static string Identifier(const string &s)
{
	auto first = s.find_first_of(" {[(&");
	return s.substr(0, first);
}


// Compile prerequisites.
static void CompilePrereqs(vector<OXMField> &fields)
{
	unordered_map<string,const OXMField*> map;
	
	for (auto &field : fields) {
		map[field.name()] = &field;
	}
	
	for (auto &field : fields) {
		vector<string> preqs = field.prereqs();
		// Find prereqStr for each preq and append it to preqresStr.
		unordered_set<string> processed;
		for (auto preq : preqs) {
			auto id = Identifier(preq);
			if (processed.find(id) == processed.end()) {
				processed.insert(id);
				auto pstr = map.find(id);
				if (pstr != map.end()) {
					if (pstr->second->hasPrereqs()) {
						field.prereqStr() = pstr->second->prereqStr() + ',' + field.prereqStr();
					}
				}
			}
		}
	}
}

static void Trim(string &s)
{
	auto first = s.find_first_not_of(" \t\n\v\f\r");
	if (first == string::npos) {
		s.clear();
	} else {
		s.erase(0, first);
		s.erase(s.find_last_not_of(" \t\n\v\f\r")+1);
	}
}

static vector<string> Split(const string &str, char delimiter, bool trim)
{
	vector<string> result;
	istringstream iss{str};
	for (string f; getline(iss, f, delimiter); ) {
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
	{ "none", nullptr, 0, false  }
};
const size_t ofp::OXMTypeInfoArraySize = 0;

