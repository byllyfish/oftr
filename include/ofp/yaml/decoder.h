#ifndef OFP_YAML_DECODER_H
#define OFP_YAML_DECODER_H

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ybyteorder.h"
#include "ofp/message.h"

namespace ofp { // <namespace ofp>
namespace yaml { // <namespace yaml>


class Decoder {
public:

	Decoder(Message *msg) : msg_{msg} {
		assert(msg->size() >= sizeof(Header));
	}

	Header *header() { return msg_->header(); }

	void decodeMsg(llvm::yaml::IO &io);

	const std::string &error() const { return error_; }

private:
	Message *msg_;
	std::string error_;
};

} // </namespace yaml>
} // </namespace ofp>


namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::yaml::Decoder> {

    static void mapping(IO &io, ofp::yaml::Decoder &decoder)
    {
    	ofp::Header *header = decoder.header();
		io.mapRequired("type", header->type_);
		io.mapRequired("xid", header->xid_);
		io.mapRequired("version", header->version_);

    	decoder.decodeMsg(io);
    }
};

} // </namespace yaml>
} // </namespace llvm>
#endif // OFP_YAML_DECODER_H
