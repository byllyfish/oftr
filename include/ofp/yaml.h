//  ===== ---- ofp/yaml.h ----------------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the YAML read and write functions.
//  ===== ------------------------------------------------------------ =====  //

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


#if 0
template <>
Exception read(const std::string &input, FlowModBuilder *msg);

template <>
std::string write(const FlowMod *msg);


template <>
Exception read(const std::string &input, SetConfigBuilder *msg);

template <>
std::string write(const SetConfig *msg);
#endif

} // </namespace yaml>
} // </namespace ofp>

#endif // OFP_OFP_YAML_H
