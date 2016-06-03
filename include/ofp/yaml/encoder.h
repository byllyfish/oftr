// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_ENCODER_H_
#define OFP_YAML_ENCODER_H_

#include "ofp/yaml/yllvm.h"

#include "ofp/yaml/ybyteorder.h"
#include "ofp/yaml/ydatapathid.h"

#include "ofp/bytelist.h"
#include "ofp/channel.h"
#include "ofp/header.h"
#include "ofp/log.h"
#include "ofp/memorychannel.h"

namespace ofp {
namespace yaml {

OFP_BEGIN_IGNORE_PADDING

class Encoder {
 public:
  using ChannelFinder =
      std::function<Channel *(const DatapathID &datapathId, UInt64 connId)>;

  explicit Encoder(ChannelFinder finder);
  Encoder(const std::string &input, bool matchPrereqsChecked = true,
          int lineNumber = 0, UInt8 defaultVersion = 0,
          ChannelFinder finder = nullptr);

  const UInt8 *data() const { return channel_.data(); }
  size_t size() const { return channel_.size(); }

  UInt64 connectionId() const { return connId_; }
  const DatapathID &datapathId() const { return datapathId_; }
  UInt8 auxiliaryId() const { return auxiliaryId_; }
  Channel *outputChannel() const { return outputChannel_; }
  UInt32 xid() const { return header_.xid(); }
  OFPMessageFlags flags() const { return flags_; }

  bool matchPrereqsChecked() const { return matchPrereqsChecked_; }

  const std::string &error() {
    errorStream_.str();
    return error_;
  }

  MemoryChannel *memoryChannel() { return &channel_; }

  bool recursive() const { return recursive_; }
  void setRecursive(bool recursive) { recursive_ = recursive; }

 private:
  MemoryChannel channel_;
  std::string error_;
  llvm::raw_string_ostream errorStream_;
  UInt64 connId_ = 0;
  DatapathID datapathId_;
  Header header_;
  OFPMultipartType subtype_ = OFPMP_UNSUPPORTED;
  OFPMessageFlags flags_ = OFP_DEFAULT_MESSAGE_FLAGS;
  ChannelFinder finder_;
  Channel *outputChannel_ = nullptr;
  int lineNumber_ = 0;
  UInt8 auxiliaryId_ = 0;
  UInt8 defaultVersion_;
  bool matchPrereqsChecked_;
  bool recursive_ = false;

  explicit Encoder(const Encoder *encoder);

  static void diagnosticHandler(const llvm::SMDiagnostic &d, void *context);
  void addDiagnostic(const llvm::SMDiagnostic &d);

  void encodeMsg(llvm::yaml::IO &io);

  friend struct llvm::yaml::MappingTraits<ofp::yaml::Encoder>;
  friend void EncodeRecursively(llvm::yaml::IO &io, const char *key,
                                ByteList &data);
};

OFP_END_IGNORE_PADDING

}  // namespace yaml
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::yaml::Encoder> {
  static void mapping(IO &io, ofp::yaml::Encoder &encoder) {
    using namespace ofp;

    Header &header = encoder.header_;
    header.setType(OFPT_UNSUPPORTED);

    // Read xid first. If present, we may need it for error reporting later.
    io.mapOptional("xid", header.xid_);
    io.mapOptional("version", header.version_);

    ofp::yaml::MessageType msgType;
    io.mapRequired("type", msgType);
    header.setType(msgType.type);
    encoder.subtype_ = msgType.subtype;

    io.mapOptional("conn_id", encoder.connId_, UInt64{});
    io.mapOptional("datapath_id", encoder.datapathId_, DatapathID{});

    UInt8 defaultAuxId = 0;
    io.mapOptional("auxiliary_id", encoder.auxiliaryId_, defaultAuxId);
    io.mapOptional("flags", encoder.flags_, OFP_DEFAULT_MESSAGE_FLAGS);

    std::string ignore;
    io.mapOptional("_file", ignore);
    io.mapOptional("_session", ignore);
    io.mapOptional("_source", ignore);
    io.mapOptional("_dest", ignore);
    io.mapOptional("time", ignore);

    if (header.type() != OFPT_UNSUPPORTED) {
      encoder.encodeMsg(io);
    }
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_ENCODER_H_
