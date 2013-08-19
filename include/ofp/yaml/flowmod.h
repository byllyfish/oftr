#ifndef OFP_YAML_FLOWMOD_H
#define OFP_YAML_FLOWMOD_H

#include "ofp/flowmod.h"
#include "ofp/exception.h"
#include "ofp/bytelist.h"

namespace ofp {  // <namespace ofp>
namespace yaml { // <namespace yaml>

/// \brief Parses YAML data from the input and produces a serialized
/// protocol message.
Exception read(const ByteRange &yaml, ByteList *output);

/// \brief Parses YAML data from the input and produces a message builder
/// object.
template <class MesgBuilderType>
Exception read(const ByteRange &yaml, MesgBuilderType *msg);

/// \brief Writes the YAML representation of a message to the output 
/// buffer.
template <class MesgType>
void write(const MesgType *msg, ByteList *yaml);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //

template <>
Exception read(const ByteRange &yaml, FlowModBuilder *msg);

template <>
void write(const FlowMod *msg, ByteList *yaml);


} // </namespace yaml>
} // </namespace ofp>

#endif // OFP_YAML_FLOWMOD_H
