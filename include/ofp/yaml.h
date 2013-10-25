//  ===== ---- ofp/yaml.h ----------------------------------*- C++ -*- =====  //
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
/// \brief Defines the YAML read and write functions.
//  ===== ------------------------------------------------------------ =====  //

// FIXME -- remove these functions and use Encoder/Decoder instead?

#ifndef OFP_OFP_YAML_H
#define OFP_OFP_YAML_H

#include "ofp/exception.h"

namespace ofp { // <namespace ofp>

class FlowMod;
class FlowModBuilder;
class SetConfig;
class SetConfigBuilder;

namespace yaml { // <namespace yaml>

/// \brief Parses YAML data from the given string and produces a message builder
/// object.
template <class MesgBuilderType>
Exception read(const std::string &input, MesgBuilderType *msg);

/// \returns the YAML representation of a protocol message.
template <class MesgType>
std::string write(const MesgType *msg);

} // </namespace yaml>
} // </namespace ofp>

#endif // OFP_OFP_YAML_H
