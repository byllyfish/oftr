//  ===== ---- ofp/yaml/encoder.h --------------------------*- C++ -*- =====  //
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
/// \brief Defines the yaml::Encoder class and its llvm::yaml::MappingTraits.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_ENCODER_H
#define OFP_YAML_ENCODER_H

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ybyteorder.h"
#include "ofp/yaml/ydatapathid.h"
#include "ofp/header.h"
#include "ofp/bytelist.h"
#include "ofp/memorychannel.h"
#include "ofp/log.h"
#include "ofp/channel.h"

namespace ofp { // <namespace ofp>
namespace yaml { // <namespace yaml>

OFP_BEGIN_IGNORE_PADDING

class Encoder {
public:
    using ChannelFinder = std::function<Channel*(const DatapathID &datapathId)>;

	Encoder(const std::string &input, ChannelFinder finder = NullChannelFinder);

	const UInt8 *data() const { return channel_.data(); }
	size_t size() const { return channel_.size(); }

    const DatapathID &datapathId() const { return datapathId_; }
    UInt8 auxiliaryId() const { return auxiliaryId_; }

	const std::string &error() { errorStream_.str(); return error_; }

private:
	MemoryChannel channel_;
	std::string error_;
    llvm::raw_string_ostream errorStream_;
    DatapathID datapathId_;
    ChannelFinder finder_;
    UInt8 auxiliaryId_ = 0;

    static void diagnosticHandler(const llvm::SMDiagnostic &diag, void *context);

    void addDiagnostic(const llvm::SMDiagnostic &diag) {
		diag.print("", errorStream_, false);
	}

    void encodeMsg(llvm::yaml::IO &io, Header &header);

    static Channel *NullChannelFinder(const DatapathID &datapathId);

    friend struct llvm::yaml::MappingTraits<ofp::yaml::Encoder>;
};

OFP_END_IGNORE_PADDING

} // </namespace yaml>
} // </namespace ofp>

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::yaml::Encoder> {

    static void mapping(IO &io, ofp::yaml::Encoder &encoder)
    {
        using namespace ofp;

    	Header header{OFPT_UNSUPPORTED};
        io.mapOptional("version", header.version_);
    	io.mapRequired("type", header.type_);
    	io.mapOptional("xid", header.xid_);
    	io.mapOptional("datapath_id", encoder.datapathId_, DatapathID{});

        UInt8 defaultAuxId = 0;
        io.mapOptional("auxiliary_id", encoder.auxiliaryId_, defaultAuxId);

    	encoder.encodeMsg(io, header);
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_ENCODER_H
