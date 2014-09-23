//  ===== ---- ofp/api/rpcevents.h -------------------------*- C++ -*- =====  //
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
/// \brief Implements ....
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_API_RPCEVENTS_H_
#define OFP_API_RPCEVENTS_H_

#include "ofp/yaml/yllvm.h"
#include "ofp/datapathid.h"
#include "ofp/yaml/ydatapathid.h"
#include "ofp/driver.h"
#include "ofp/padding.h"
#include "ofp/yaml/ybytelist.h"
#include "ofp/yaml/yaddress.h"
#include "ofp/yaml/encoder.h"

namespace ofp {
namespace api {

/// Indicates that `id` is missing from RPC message.
const UInt64 RPC_ID_MISSING = 0xffffffffffffffffUL;

/// RPC Methods
enum RpcMethod : UInt32 {
  METHOD_LISTEN = 0,    // ofp.listen
  METHOD_CONNECT,       // ofp.connect
  METHOD_CLOSE,         // ofp.close
  METHOD_SEND,          // ofp.send
  METHOD_CONFIG,        // ofp.config
  METHOD_DATAPATH,      // ofp.datapath
  METHOD_MESSAGE,       // ofp.message
  METHOD_MESSAGE_ERROR, // ofp.message_error
  METHOD_LIST_CONNS,    // ofp.list_connections
  METHOD_UNSUPPORTED
};

/// JSON-RPC error codes.
enum RpcErrorCode {
  ERROR_CODE_INVALID_REQUEST = -32600,
  ERROR_CODE_METHOD_NOT_FOUND = -32601,
  ERROR_CODE_DATAPATH_NOT_FOUND = -65000,
  ERROR_CODE_INVALID_OPTION = -65001,
};

OFP_BEGIN_IGNORE_PADDING

/// Represents the general JSON-RPC error response.
struct RpcErrorResponse {
  explicit RpcErrorResponse(UInt64 ident) : id{ident} {}
  std::string toJson();

  struct Error {
    int code;
    std::string message;
  };

  UInt64 id;
  Error error;
};

/// Represents an object in a ofp.list_connections result list.
struct RpcConnectionStats {
  IPv6Endpoint localEndpoint;
  IPv6Endpoint remoteEndpoint;
  UInt64 connId;
  DatapathID datapathId;
  UInt8 auxiliaryId;
  ChannelTransport transport;
};

//---------------------//
// o f p . l i s t e n //
//---------------------//

/// Represents a RPC request to listen for new connections (METHOD_LISTEN)
struct RpcListen {
  explicit RpcListen(UInt64 ident) : id{ident} {}

  struct Params {
    /// Endpoint to listen on.
    IPv6Endpoint endpoint;
    /// Array of options.
    std::vector<std::string> options;
  };

  UInt64 id;
  Params params;
};

/// Represents a RPC response to listen for new connections (METHOD_LISTEN)
struct RpcListenResponse {
  explicit RpcListenResponse(UInt64 ident) : id{ident} {}
  std::string toJson();

  struct Result {
    /// Connection ID of listening connection.
    UInt64 connId;
  };

  UInt64 id;
  Result result;
};

//-----------------------//
// o f p . c o n n e c t //
//-----------------------//

/// Represents a RPC request to make an outgoing connection (METHOD_CONNECT)
struct RpcConnect {
  explicit RpcConnect(UInt64 ident) : id{ident} {}

  struct Params {
    /// Endpoint to connect to.
    IPv6Endpoint endpoint;
    /// Array of options.
    std::vector<std::string> options;
  };

  UInt64 id;
  Params params;
};

/// Represents a RPC response to make an outgoing connection (METHOD_CONNECT)
struct RpcConnectResponse {
  explicit RpcConnectResponse(UInt64 ident) : id{ident} {}
  std::string toJson();

  struct Result {
    /// Connection ID of outgoing connection.
    UInt64 connId;
  };

  UInt64 id;
  Result result;
};

//-------------------//
// o f p . c l o s e //
//-------------------//

/// Represents a RPC request to close a connection (METHOD_CLOSE)
struct RpcClose {
  explicit RpcClose(UInt64 ident) : id{ident} {}

  struct Params {
    /// Connection ID to close.
    UInt64 connId;
  };

  UInt64 id;
  Params params;
};

/// Represents a RPC response to close a connection (METHOD_CLOSE)
struct RpcCloseResponse {
  explicit RpcCloseResponse(UInt64 ident) : id{ident} {}
  std::string toJson();

  struct Result {
    /// Count of closed connections.
    UInt32 count;
  };

  UInt64 id;
  Result result;
};

//-----------------------------------------//
// o f p . l i s t _ c o n n e c t i o n s //
//-----------------------------------------//

/// Represents a RPC request to list connection stats (METHOD_LIST_CONNS)
struct RpcListConns {
  explicit RpcListConns(UInt64 ident) : id{ident} {}

  struct Params {
    UInt64 connId;
  };

  UInt64 id;
  Params params;
};

struct RpcListConnsResponse {
  explicit RpcListConnsResponse(UInt64 ident) : id{ident} {}
  std::string toJson();

  using Result = std::vector<RpcConnectionStats>;

  UInt64 id;
  Result result;
};

//-----------------//
// o f p . s e n d //
//-----------------//

/// Represents a RPC request to send a message to datapath (METHOD_SEND).
struct RpcSend {
  explicit RpcSend(UInt64 ident, yaml::Encoder::ChannelFinder finder) : id{ident}, params{finder} {}

  UInt64 id;
  yaml::Encoder params;
};

/// Represnts a RPC response to send a message to datapath (METHOD_SEND).
struct RpcSendResponse {
  explicit RpcSendResponse(UInt64 ident) : id{ident} {}
  std::string toJson();

  struct Result {
    UInt64 connId;
    ByteRange data;
  };

  UInt64 id;
  Result result;
};

//---------------------//
// o f p . c o n f i g //
//---------------------//

/// Represents a RPC request to configure some options (METHOD_CONFIG).
struct RpcConfig {
  explicit RpcConfig(UInt64 ident) : id{ident} {}

  struct Params {
    std::vector<std::string> options;
  };

  UInt64 id;
  Params params;
};

/// Represents a RPC response to configure some options (METHOD_CONFIG).
struct RpcConfigResponse {
  explicit RpcConfigResponse(UInt64 ident) : id{ident} {}
  std::string toJson();

  struct Result {
    std::vector<std::string> options;
  };

  UInt64 id;
  Result result;
};

//-------------------------//
// o f p . d a t a p a t h //
//-------------------------//

/// Represents a RPC notification about a datapath (METHOD_DATAPATH)
struct RpcDatapath {
  std::string toJson();

  struct Params {
    UInt64 connId;
    DatapathID datapathId;
    std::string status;
    UInt8 version{};
  };

  Params params;
};

//-----------------------------------//
// o f p . m e s s a g e _ e r r o r //
//-----------------------------------//

/// Represents a RPC notification about an incoming message error (METHOD_MESSAGE_ERROR).
struct RpcMessageError {
  std::string toJson();

  struct Params {
    DatapathID datapathId;
    std::string error;
    ByteRange data;
  };

  Params params;
};

OFP_END_IGNORE_PADDING

}  // namespace api
}  // namespace ofp

LLVM_YAML_IS_SEQUENCE_VECTOR(ofp::api::RpcConnectionStats);

namespace llvm {
namespace yaml {


template <>
struct ScalarTraits<ofp::api::RpcMethod> {
  static ofp::yaml::EnumConverter<ofp::api::RpcMethod> converter;

  static void output(const ofp::api::RpcMethod &value, void *ctxt,
                     llvm::raw_ostream &out) {
    llvm::StringRef scalar;
    if (converter.convert(value, &scalar)) {
      out << scalar;
    } else {
      out << format("0x%02X", value);
    }
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::api::RpcMethod &value) {
    if (!converter.convert(scalar, &value)) {
      value = ofp::api::METHOD_UNSUPPORTED;
      return "unknown method";
    }
    
    return "";
  }
};

template <>
struct ScalarEnumerationTraits<ofp::ChannelTransport> {
  static void enumeration(IO &io, ofp::ChannelTransport &value) {
    io.enumCase(value, "NONE", ofp::ChannelTransport::None);
    io.enumCase(value, "TCP", ofp::ChannelTransport::TCP_Plaintext);
    io.enumCase(value, "UDP", ofp::ChannelTransport::UDP_Plaintext);
    io.enumCase(value, "TLS", ofp::ChannelTransport::TCP_TLS);
    io.enumCase(value, "DTLS", ofp::ChannelTransport::UDP_DTLS);
  }
};

template <>
struct MappingTraits<ofp::api::RpcListen::Params> {
  static void mapping(IO &io, ofp::api::RpcListen::Params &params) {
    io.mapRequired("endpoint", params.endpoint);
    io.mapOptional("options", params.options);
  }
};

template <>
struct MappingTraits<ofp::api::RpcConnect::Params> {
  static void mapping(IO &io, ofp::api::RpcConnect::Params &params) {
    io.mapRequired("endpoint", params.endpoint);
    io.mapOptional("options", params.options);
  }
};

template <>
struct MappingTraits<ofp::api::RpcClose::Params> {
  static void mapping(IO &io, ofp::api::RpcClose::Params &params) {
    io.mapRequired("conn_id", params.connId);
  }
};

template <>
struct MappingTraits<ofp::api::RpcConfig::Params> {
  static void mapping(IO &io, ofp::api::RpcConfig::Params &params) {
    io.mapRequired("options", params.options);
  }
};

template <>
struct MappingTraits<ofp::api::RpcListConns::Params> {
  static void mapping(IO &io, ofp::api::RpcListConns::Params &result) {
    io.mapRequired("conn_id", result.connId);
  }
};

template <>
struct MappingTraits<ofp::api::RpcListenResponse> {
  static void mapping(IO &io, ofp::api::RpcListenResponse &response) {
    io.mapRequired("id", response.id);
    io.mapRequired("result", response.result);
  }
};

template <>
struct MappingTraits<ofp::api::RpcListenResponse::Result> {
  static void mapping(IO &io, ofp::api::RpcListenResponse::Result &result) {
    io.mapRequired("conn_id", result.connId);
  }
};

template <>
struct MappingTraits<ofp::api::RpcConnectResponse> {
  static void mapping(IO &io, ofp::api::RpcConnectResponse &response) {
    io.mapRequired("id", response.id);
    io.mapRequired("result", response.result);
  }
};

template <>
struct MappingTraits<ofp::api::RpcConnectResponse::Result> {
  static void mapping(IO &io, ofp::api::RpcConnectResponse::Result &result) {
    io.mapRequired("conn_id", result.connId);
  }
};

template <>
struct MappingTraits<ofp::api::RpcCloseResponse> {
  static void mapping(IO &io, ofp::api::RpcCloseResponse &response) {
    io.mapRequired("id", response.id);
    io.mapRequired("result", response.result);
  }
};

template <>
struct MappingTraits<ofp::api::RpcCloseResponse::Result> {
  static void mapping(IO &io, ofp::api::RpcCloseResponse::Result &result) {
    io.mapRequired("count", result.count);
  }
};

template <>
struct MappingTraits<ofp::api::RpcListConnsResponse> {
  static void mapping(IO &io, ofp::api::RpcListConnsResponse &response) {
    io.mapRequired("id", response.id);
    io.mapRequired("result", response.result);
  }
};

template <>
struct MappingTraits<ofp::api::RpcConnectionStats> {
  static void mapping(IO &io, ofp::api::RpcConnectionStats &stats) {
    io.mapRequired("local_endpoint", stats.localEndpoint);
    io.mapRequired("remote_endpoint", stats.remoteEndpoint);
    io.mapRequired("datapath_id", stats.datapathId);
    io.mapRequired("conn_id", stats.connId);
    io.mapRequired("auxiliary_id", stats.auxiliaryId);
    io.mapRequired("transport", stats.transport);
  }
};

template <>
struct MappingTraits<ofp::api::RpcSendResponse> {
  static void mapping(IO &io, ofp::api::RpcSendResponse &response) {
    io.mapRequired("id", response.id);
    io.mapRequired("result", response.result);
  }
};

template <>
struct MappingTraits<ofp::api::RpcSendResponse::Result> {
  static void mapping(IO &io, ofp::api::RpcSendResponse::Result &result) {
    io.mapRequired("conn_id", result.connId);
    io.mapRequired("data", result.data);
  }
};

template <>
struct MappingTraits<ofp::api::RpcErrorResponse> {
  static void mapping(IO &io, ofp::api::RpcErrorResponse &response) {
    io.mapRequired("id", response.id);
    io.mapRequired("error", response.error);
  }
};

template <>
struct MappingTraits<ofp::api::RpcErrorResponse::Error> {
  static void mapping(IO &io, ofp::api::RpcErrorResponse::Error &error) {
    io.mapRequired("code", error.code);
    io.mapRequired("message", error.message);
  }
};

template <>
struct MappingTraits<ofp::api::RpcDatapath> {
  static void mapping(IO &io, ofp::api::RpcDatapath &response) {
    ofp::api::RpcMethod method = ofp::api::METHOD_DATAPATH;
    io.mapRequired("method", method);
    io.mapRequired("params", response.params);
  }
};

template <>
struct MappingTraits<ofp::api::RpcDatapath::Params> {
  static void mapping(IO &io, ofp::api::RpcDatapath::Params &params) {
    io.mapRequired("conn_id", params.connId);
    io.mapRequired("datapath_id", params.datapathId);
    io.mapRequired("version", params.version);
    io.mapRequired("status", params.status);
  }
};

template <>
struct MappingTraits<ofp::api::RpcMessageError> {
  static void mapping(IO &io, ofp::api::RpcMessageError &response) {
    ofp::api::RpcMethod method = ofp::api::METHOD_MESSAGE_ERROR;
    io.mapRequired("method", method);
    io.mapRequired("params", response.params);
  }
};

template <>
struct MappingTraits<ofp::api::RpcMessageError::Params> {
  static void mapping(IO &io, ofp::api::RpcMessageError::Params &params) {
    io.mapRequired("datapath_id", params.datapathId);
    io.mapRequired("error", params.error);
    io.mapRequired("data", params.data);
  }
};


}  // namespace yaml
}  // namespace llvm

#endif  // OFP_API_RPCEVENTS_H_
