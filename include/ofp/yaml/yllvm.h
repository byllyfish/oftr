//  ===== ---- ofp/yaml/yllvm.h ----------------------------*- C++ -*- =====  //
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
/// \brief Include LLVM YAML headers and define yaml traits for common types.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YLLVM_H_
#define OFP_YAML_YLLVM_H_

// The following two definitions are required by llvm/Support/DataTypes.h
#define __STDC_LIMIT_MACROS 1
#define __STDC_CONSTANT_MACROS 1

#include "llvm/Support/YAMLTraits.h"
#include "llvm/Support/YAMLParser.h"
#include "ofp/yaml/yconstants.h"
#include "ofp/header.h"

LLVM_YAML_IS_FLOW_SEQUENCE_VECTOR(ofp::UInt8);

#endif  // OFP_YAML_YLLVM_H_
