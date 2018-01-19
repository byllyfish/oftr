// Copyright (c) 2016-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_GETJSON_H_
#define OFP_YAML_GETJSON_H_

#include <istream>
#include "ofp/types.h"

namespace ofp {
namespace yaml {

/// Common function pointer type for exported functions.

using GetMsgFunction = bool (*)(std::istream &, std::string &, int &, int &);

/// Read the next complete JSON object from the input stream.
///
/// This function ignores anything not contained in '{' and '}',
/// including identifiers, string literals, punctuation and other tokens. It
/// will process an array of objects as separate messages. It just splits the
/// input up into objects; it doesn't validate the JSON syntax.

bool getjson(std::istream &input, std::string &json, int &lineNum,
             int &newlineCount);

/// Read the next YAML message from the input stream.
///
/// YAML messages are delimited by "---" or "..." on a line by themselves. If
/// the input lines are delimited by CR-LF, the CR-LF is converted to LF.

bool getyaml(std::istream &input, std::string &yaml, int &lineNum,
             int &newlineCount);

/// Read the next non-empty line from the input stream.

bool getline(std::istream &input, std::string &line, int &lineNum,
             int &newlineCount);

}  // namespace yaml
}  // namespace ofp

#endif  // OFP_YAML_GETJSON_H_
