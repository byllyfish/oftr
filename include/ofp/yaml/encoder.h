// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_ENCODER_H_
#define OFP_YAML_ENCODER_H_

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ybyteorder.h"
#include "ofp/yaml/ydatapathid.h"
#include "ofp/header.h"
#include "ofp/bytelist.h"
#include "ofp/memorychannel.h"
#include "ofp/log.h"
#include "ofp/channel.h"

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
  llvm::yaml::Input *io() const { return io_; }

  bool matchPrereqsChecked() const { return matchPrereqsChecked_; }

  const std::string &error() {
    errorStream_.str();
    return error_;
  }

 private:
  MemoryChannel channel_;
  std::string error_;
  llvm::raw_string_ostream errorStream_;
  llvm::yaml::Input *io_ = nullptr;
  UInt64 connId_ = 0;
  DatapathID datapathId_;
  Header header_;
  OFPMultipartType subtype_ = OFPMP_UNSUPPORTED;
  OFPMultipartFlags flags_ = OFPMPF_NONE;
  ChannelFinder finder_;
  Channel *outputChannel_ = nullptr;
  int lineNumber_ = 0;
  UInt8 auxiliaryId_ = 0;
  UInt8 defaultVersion_;
  bool matchPrereqsChecked_;

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

    io.mapOptional("version", header.version_);

    ofp::yaml::MessageType msgType;
    io.mapRequired("type", msgType);
    header.setType(msgType.type);
    encoder.subtype_ = msgType.subtype;

    io.mapOptional("xid", header.xid_);
    io.mapOptional("conn_id", encoder.connId_, UInt64{});
    io.mapOptional("datapath_id", encoder.datapathId_, DatapathID{});

    UInt8 defaultAuxId = 0;
    io.mapOptional("auxiliary_id", encoder.auxiliaryId_, defaultAuxId);
    io.mapOptional("flags", encoder.flags_, OFPMPF_NONE);

    std::string ignore;
    io.mapOptional("_file", ignore);
    
    if (header.type() != OFPT_UNSUPPORTED) {
      encoder.encodeMsg(io);
    }
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_ENCODER_H_
