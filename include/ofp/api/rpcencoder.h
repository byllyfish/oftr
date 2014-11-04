// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_API_RPCENCODER_H_
#define OFP_API_RPCENCODER_H_

#include "ofp/yaml/yllvm.h"
#include "ofp/api/rpcevents.h"

namespace ofp {
namespace api {

class ApiConnection;

/// Parse YAML RPC events and pass them to the connection as event structures.
/// If there is an error parsing the YAML, write an OFPLIB_PROTOCOL_ERROR event
/// back to the connection. This class is called an 'encoder' since it
/// translates from YAML to an internal binary representation.

OFP_BEGIN_IGNORE_PADDING

class RpcEncoder {
 public:
  explicit RpcEncoder(const std::string &input, ApiConnection *conn,
                      yaml::Encoder::ChannelFinder finder);

  const std::string &error() {
    errorStream_.str();
    return error_;
  }

 private:
  ApiConnection *conn_;
  std::string error_;
  llvm::raw_string_ostream errorStream_;
  yaml::Encoder::ChannelFinder finder_;
  std::string jsonrpc_;
  llvm::Optional<ofp::UInt64> id_;
  RpcMethod method_ = ofp::api::METHOD_UNSUPPORTED;

  static void diagnosticHandler(const llvm::SMDiagnostic &diag, void *context);
  void addDiagnostic(const llvm::SMDiagnostic &diag);

  void encodeParams(llvm::yaml::IO &io);

  friend struct llvm::yaml::MappingTraits<ofp::api::RpcEncoder>;
};

OFP_END_IGNORE_PADDING

}  // namespace api
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::api::RpcEncoder> {
  static void mapping(IO &io, ofp::api::RpcEncoder &encoder) {
    io.mapOptional("jsonrpc", encoder.jsonrpc_);
    io.mapOptional("id", encoder.id_);
    io.mapRequired("method", encoder.method_);

    encoder.encodeParams(io);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_API_RPCENCODER_H_
