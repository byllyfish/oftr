// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/yaml/yschema.h"

#include <sstream>

namespace llvm {

inline std::ostream &operator<<(std::ostream &os, StringRef s) {
  return os.write(s.data(), ofp::Signed_cast(s.size()));
}

}  // namespace llvm

using namespace ofp::yaml;

const char *const kMessagePrefix = R"""(  version: !opt UInt8
  datapath_id: !opt DatapathID
  xid: !opt UInt32
  conn_id: !opt UInt64
  auxiliary_id: !opt UInt8
  flags: !optout [MultipartFlags]
  time: !optout Timestamp
  src: !optout IPEndpoint
  dst: !optout IPEndpoint
  _file: !optout String
)""";

const char *const kBriefMessagePrefix = "  datapath_id: !opt DatapathID\n";

static bool equalsAlnum(llvm::StringRef lhs, llvm::StringRef rhs);
static std::string stringJoin(const std::vector<llvm::StringRef> &vec,
                              const char *sep);
static void replaceAll(std::string &str, const std::string &from,
                       const std::string &to);
static std::string unsignedTypeName(size_t size);
static std::string unsignedTypeEnum(size_t size);
static bool isTypeName(llvm::StringRef s);

bool Schema::equals(llvm::StringRef s) const {
  return equalsAlnum(name_, s) || equalsAlnum(type_, s);
}

/// Return a set of schemas that this schema depends on.
///
/// This is implemented by scanning each line of the schema value looking for
/// the pattern "key: Type ..." where Type starts with a capital letter, but
/// is not all caps. This function also checks for a type enclosed in brackets,
/// and recognizes YAML tags. (e.g. "key: !tag Type").
std::set<std::string> Schema::dependsOnSchemas() const {
  std::set<std::string> result;
  std::stringstream iss{value_.str()};

  std::string line;
  while (std::getline(iss, line)) {
    auto pair = llvm::StringRef{line}.split(':');
    if (!pair.second.empty()) {
      auto type = pair.second.trim();
      auto endType = type.find_first_of(" \t\n\v\f\r]");
      // If the first word begins with '!', get the second word.
      if (type.startswith("!")) {
        type = type.drop_front(endType < type.size() ? endType : type.size());
        type = type.ltrim();
        endType = type.find_first_of(" \t\n\v\f\r]");
      }
      if (endType < type.size()) {
        type = type.drop_back(type.size() - endType);
      }
      if (type.startswith("[")) {
        type = type.drop_front(1);
      }
      if (isTypeName(type)) {
        result.insert(type.str());
      }
    }
  }

  return result;
}

void Schema::print(std::ostream &os, bool brief) const {
  os << type() << '/' << name().str() << ": ";
  if (isObject_) {
    os << '\n';
    if (type().equals("Message")) {
      if (brief) {
        os << kBriefMessagePrefix;
      } else {
        os << kMessagePrefix;
      }
    }
    printValue(os, 2);
  } else {
    printValue(os);
  }
  os << '\n';
}

void Schema::printValue(std::ostream &os, unsigned indent) const {
  std::string val = value().str();

  if (indent > 0) {
    std::string spaces(indent, ' ');
    val = spaces + val;
    replaceAll(val, "\n", "\n" + spaces);
  }

  os << val;
}

void Schema::init(const char *schema) {
  // Split first line from the rest.
  auto p = llvm::StringRef{schema}.split('\n');

  // Set value to the rest.
  value_ = p.second;

  //  If the first line is enclosed in braces, we trim them and set isObject_
  //  to true.
  if (p.first.startswith("{")) {
    p.first = p.first.ltrim("{").rtrim("}");
    isObject_ = true;
  }

  // Parse the first line of form `type/name` or just `name`.
  std::tie(type_, name_) = p.first.split('/');
  if (name_.empty()) {
    std::swap(type_, name_);
  }
}

std::string Schema::MakeSchemaString(const char *const name,
                                     const std::vector<llvm::StringRef> &values,
                                     size_t size) {
  std::string result = stringJoin(values, " | ");
  auto line = llvm::StringRef{name};

  if (line.startswith_lower("mixed/")) {
    result = unsignedTypeName(size) + " | " + result;
  } else if (line.startswith_lower("enum/")) {
    result += " | " + unsignedTypeEnum(size);
  }

  return std::string(name) + "\n" + result + "\n";
}

std::string Schema::MakeFlagSchemaString(const char *name,
                                         const std::string &values,
                                         size_t size) {
  llvm::SmallVector<llvm::StringRef, 25> vals;
  llvm::StringRef{values}.split(vals, ",");

  std::vector<llvm::StringRef> words;
  for (auto val : vals) {
    if (!val.ltrim().startswith("'0x"))
      words.push_back(val.trim());
  }

  // Sometimes `name` will be formatted as "type/name=VAL1,VAL2,...,VALN". Check
  // for this syntax and include the extra values in the list. This is used
  // to include values that consist of multiple masked bits.
  llvm::StringRef key;
  llvm::StringRef extraValues;
  std::tie(key, extraValues) = llvm::StringRef{name}.split('=');

  if (!extraValues.empty()) {
    vals.clear();
    llvm::StringRef{extraValues}.split(vals, ",");
    for (auto val : vals) {
      words.push_back(val.trim());
    }
  }

  std::string result = stringJoin(words, " | ");
  result += " | " + unsignedTypeEnum(size);

  return std::string(key) + '\n' + result + '\n';
}

// Compare two strings case insensitively skipping non-alphanumeric chars.
static bool equalsAlnum(llvm::StringRef lhs, llvm::StringRef rhs) {
  size_t i = 0;
  size_t j = 0;

  while (i < lhs.size() && j < rhs.size()) {
    if (!std::isalnum(lhs[i])) {
      ++i;
      continue;
    }
    if (!std::isalnum(rhs[j])) {
      ++j;
      continue;
    }
    if (std::tolower(lhs[i]) != std::tolower(rhs[j])) {
      return false;
    }
    ++i;
    ++j;
  }

  return i >= lhs.size() && j >= rhs.size();
}

static std::string stringJoin(const std::vector<llvm::StringRef> &vec,
                              const char *sep) {
  std::string result;
  if (vec.empty())
    return result;
  for (size_t i = 0; i < vec.size() - 1; ++i) {
    result += vec[i];
    result += sep;
  }
  result += vec[vec.size() - 1];
  return result;
}

static void replaceAll(std::string &str, const std::string &from,
                       const std::string &to) {
  size_t start = 0;
  while ((start = str.find(from, start)) != std::string::npos) {
    str.replace(start, from.length(), to);
    start += to.length();
  }
}

/// Return a string that represents an unsigned type of the specified size.
static std::string unsignedTypeName(size_t size) {
  switch (size) {
    case 1:
      return "UInt8";
    case 2:
      return "UInt16";
    case 4:
      return "UInt32";
    case 8:
      return "UInt64";
    default:
      return "UIntXX";
  }
}

/// Return a string that represents an hexadecimal enum representation.
static std::string unsignedTypeEnum(size_t size) {
  switch (size) {
    case 1:
      return "Hex8";
    case 2:
      return "Hex16";
    case 4:
      return "Hex32";
    case 8:
      return "Hex64";
    default:
      return "HexXX";
  }
}

/// Return true if s begins with a capital letter, but is _not_ all caps.
/// Ignores underscore and hyphen.
static bool isTypeName(llvm::StringRef s) {
  if (s.empty() || !std::isalpha(s[0]) || !std::isupper(s[0]))
    return false;

  for (char ch : s) {
    if (ch == '_' || ch == '-')
      continue;
    if (std::isalpha(ch) && !std::isupper(ch)) {
      return true;
    }
  }

  return false;
}
