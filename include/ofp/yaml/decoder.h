//  ===== ---- ofp/yaml/decoder.h --------------------------*- C++ -*- =====  //
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
/// \brief Defines the yaml::Decoder class and its llvm::yaml::MappingTraits.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_DECODER_H_
#define OFP_YAML_DECODER_H_

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ybyteorder.h"
#include "ofp/yaml/ydatapathid.h"
#include "ofp/message.h"
#include "ofp/channel.h"

namespace ofp {
namespace yaml {

class Decoder {
public:

    explicit Decoder(const Message *msg, bool useJsonFormat = false);

    const llvm::StringRef result() const
    {
        return result_;
    }

    const std::string &error() const
    {
        return error_;
    }

private:
    const Message *msg_;
    llvm::SmallString<1024> result_;
    std::string error_;

    bool decodeMsg(llvm::yaml::IO &io);

    friend struct llvm::yaml::MappingTraits<ofp::yaml::Decoder>;
};

}  // namespace yaml
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::yaml::Decoder> {

    static void mapping(IO &io, ofp::yaml::Decoder &decoder)
    {
        using namespace ofp;

        Header header = *decoder.msg_->header();
        assert(header.length() == decoder.msg_->size());

        io.mapRequired("type", header.type_);
        io.mapRequired("xid", header.xid_);
        io.mapRequired("version", header.version_);

        Channel *source = decoder.msg_->source();
        if (source) {
            DatapathID dpid = source->datapathId();
            io.mapRequired("datapath_id", dpid);

            Hex8 auxID = source->auxiliaryId();
            if (auxID) {
                io.mapRequired("auxiliary_id", auxID);
            }
        }

        if (!decoder.decodeMsg(io)) {
            decoder.error_ = "Invalid data.";
        }
    }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_DECODER_H_
