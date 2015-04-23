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
          int lineNumber = 0, UInt8 defaultVersion = 0, ChannelFinder finder = NullChannelFinder);

  const UInt8 *data() const { return channel_.data(); }
  size_t size() const { return channel_.size(); }

  UInt64 connectionId() const { return connId_; }
  const DatapathID &datapathId() const { return datapathId_; }
  UInt8 auxiliaryId() const { return auxiliaryId_; }
  Channel *outputChannel() const { return outputChannel_; }

  bool matchPrereqsChecked() const { return matchPrereqsChecked_; }

  const std::string &error() {
    errorStream_.str();
    return error_;
  }

 private:
  MemoryChannel channel_;
  std::string error_;
  llvm::raw_string_ostream errorStream_;
  UInt64 connId_ = 0;
  DatapathID datapathId_;
  Header header_;
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

  static Channel *NullChannelFinder(const DatapathID &datapathId,
                                    UInt64 connId);

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
    io.mapRequired("type", header.type_);
    io.mapOptional("xid", header.xid_);

    io.mapOptional("conn_id", encoder.connId_, UInt64{});
    io.mapOptional("datapath_id", encoder.datapathId_, DatapathID{});

    UInt8 defaultAuxId = 0;
    io.mapOptional("auxiliary_id", encoder.auxiliaryId_, defaultAuxId);

    encoder.encodeMsg(io);
  }

  static StringRef validate(IO &io, ofp::yaml::Encoder &encoder) {
    if (encoder.header_.type() == ofp::OFPT_UNSUPPORTED) {
      return "";
    }
    if (encoder.header_.version() == 0 && encoder.header_.type() != ofp::OFPT_HELLO) {
      return "protocol version unspecified";
    }
    if (!encoder.header_.validateVersionAndType()) {
      return "invalid combination of version and type";
    }
    return "";
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_ENCODER_H_
