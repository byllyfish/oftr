#ifndef OFP_YAML_OFP_YAML_H
#define OFP_YAML_OFP_YAML_H

#include "ofp/exception.h"

namespace ofp { // <namespace ofp>
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

#endif // OFP_YAML_OFP_YAML_H
