#ifndef OFP_YAML_FLOWMOD_H
#define OFP_YAML_FLOWMOD_H

#include "ofp/yaml/ofp_yaml.h"
#include "ofp/flowmod.h"
#include "ofp/exception.h"
#include "ofp/bytelist.h"

namespace ofp {  // <namespace ofp>
namespace yaml { // <namespace yaml>

template <>
Exception read(const std::string &input, FlowModBuilder *msg);

template <>
std::string write(const FlowMod *msg);


} // </namespace yaml>
} // </namespace ofp>

#endif // OFP_YAML_FLOWMOD_H
