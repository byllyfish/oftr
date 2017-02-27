// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_DECODER_H_
#define OFP_YAML_DECODER_H_

#include "ofp/yaml/yllvm.h"

#include "ofp/yaml/yaddress.h"
#include "ofp/yaml/ybyteorder.h"
#include "ofp/yaml/ydatapathid.h"
#include "ofp/yaml/ytimestamp.h"

#include "ofp/channel.h"
#include "ofp/message.h"
#include "ofp/messageinfo.h"

namespace ofp {
namespace yaml {

class Decoder {
 public:
  explicit Decoder(const Message *msg, bool useJsonFormat = false,
                   bool includePktMatch = false);

  const llvm::StringRef result() const { return result_; }

  const std::string &error() const { return error_; }
  void setError(const std::string &error) { error_ = error; }
  
 private:
  const Message *msg_;
  llvm::SmallString<1024> result_;
  std::string error_;

  explicit Decoder(const Message *msg, const Decoder *decoder);

  bool decodeMsg(llvm::yaml::IO &io);
  bool decodeRequestForward(llvm::yaml::IO &io, const Message *msg);

  friend struct llvm::yaml::MappingTraits<ofp::yaml::Decoder>;
  friend void DecodeRecursively(llvm::yaml::IO &io, const char *key,
                                const Message *msg);
};

}  // namespace yaml
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::yaml::Decoder> {
  static void mapping(IO &io, ofp::yaml::Decoder &decoder) {
    using namespace ofp;

    Header header = *decoder.msg_->header();
    assert(header.length() == decoder.msg_->size());

    ofp::MessageType msgType = decoder.msg_->msgType();
    io.mapRequired("type", msgType);

    Timestamp time = decoder.msg_->time();
    if (time.valid()) {
      io.mapRequired("time", time);
    }

    if (msgType.type() == OFPT_MULTIPART_REQUEST ||
        msgType.type() == OFPT_MULTIPART_REPLY) {
      OFPMultipartFlags flags = decoder.msg_->flags();
      io.mapRequired("flags", flags);
    }

    io.mapRequired("xid", header.xid_);
    io.mapRequired("version", header.version_);

    Channel *source = decoder.msg_->source();
    if (source) {
      Hex64 connId = source->connectionId();
      if (connId) {
        io.mapRequired("conn_id", connId);
      }

      DatapathID dpid = source->datapathId();
      if (!dpid.empty()) {
        io.mapRequired("datapath_id", dpid);
      }

      Hex8 auxID = source->auxiliaryId();
      if (auxID) {
        io.mapRequired("auxiliary_id", auxID);
      }
    }

    const MessageInfo *info = decoder.msg_->info();
    if (info && info->available()) {
      UInt64 sessionId = info->sessionId();
      if (sessionId) {
        // Export the sessionId in the the `conn_id` property. We do not
        // expect this to conflict with the use by `source` above.
        assert(source == nullptr);
        IPv6Endpoint src = info->source();
        IPv6Endpoint dst = info->dest();
        io.mapRequired("conn_id", sessionId);
        io.mapRequired("src", src);
        io.mapRequired("dst", dst);
      }
      if (!info->filename().empty()) {
        std::string filename = info->filename();
        io.mapRequired("_file", filename);
      }
    }

    if (!decoder.decodeMsg(io)) {
      decoder.error_ = info ? info->errorMessage() : "Invalid data.";
    }
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_DECODER_H_
