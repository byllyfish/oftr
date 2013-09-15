#ifndef OFP_YAML_APIENCODER_H
#define OFP_YAML_APIENCODER_H

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/apievents.h"

namespace ofp { // <namespace ofp>
namespace yaml { // <namespace yaml>

class ApiConnection;

/// Parse YAML API events and pass them to the connection as event structures.
/// If there is an error parsing the YAML, write a OFPLIB_PROTOCOL_ERROR event
/// back to the connection. This class is called an 'encoder' since it
/// translates from YAML to an internal binary representation.
class ApiEncoder {
public:
	ApiEncoder(const std::string &input, ApiConnection *conn);

private:
	ApiConnection *conn_;
	std::string error_;
    llvm::raw_string_ostream errorStream_;

    static void diagnosticHandler(const llvm::SMDiagnostic &diag, void *context);

    void addDiagnostic(const llvm::SMDiagnostic &diag) {
		diag.print("", errorStream_, false);
	}

    void encodeMsg(llvm::yaml::IO &io, ApiEvent event);

    friend struct llvm::yaml::MappingTraits<ofp::yaml::ApiEncoder>;
};

} // </namespace yaml>
} // </namespace ofp>


namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::yaml::ApiEncoder> {

    static void mapping(IO &io, ofp::yaml::ApiEncoder &encoder)
    {
        using namespace ofp::yaml;

    	ApiEvent event = LIBOFP_INVALID;
    	io.mapRequired("event", event);
    	encoder.encodeMsg(io, event);
    }
};

} // </namespace yaml>
} // </namespace llvm>
#endif // OFP_YAML_APIENCODER_H
