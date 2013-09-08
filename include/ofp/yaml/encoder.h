#ifndef OFP_YAML_ENCODER_H
#define OFP_YAML_ENCODER_H

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ybyteorder.h"
#include "ofp/header.h"
#include "ofp/bytelist.h"
#include "ofp/memorychannel.h"
#include "ofp/log.h"

namespace ofp { // <namespace ofp>
namespace yaml { // <namespace yaml>

class Encoder {
public:

	Encoder(const std::string &input);

	const UInt8 *data() const { return channel_.data(); }
	size_t size() const { return channel_.size(); }

	const std::string &error() { errorStream_.str(); return error_; }

private:
	MemoryChannel channel_;
	std::string error_;
    llvm::raw_string_ostream errorStream_;

    static void diagnosticHandler(const llvm::SMDiagnostic &diag, void *context);

    void addDiagnostic(const llvm::SMDiagnostic &diag) {
		diag.print("", errorStream_, false);
	}

    void encodeMsg(llvm::yaml::IO &io, Header &header);

    friend struct llvm::yaml::MappingTraits<ofp::yaml::Encoder>;
};

} // </namespace yaml>
} // </namespace ofp>

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::yaml::Encoder> {

    static void mapping(IO &io, ofp::yaml::Encoder &encoder)
    {
    	ofp::Header header{ofp::OFPT_UNSUPPORTED};
    	io.mapRequired("type", header.type_);
    	io.mapRequired("xid", header.xid_);
    	io.mapRequired("version", header.version_);
    	encoder.encodeMsg(io, header);
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_ENCODER_H
