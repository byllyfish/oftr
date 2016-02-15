// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_RPC_RPCEVENTS_H_
#define OFP_RPC_RPCEVENTS_H_

#include "ofp/yaml/yllvm.h"
#include "ofp/datapathid.h"
#include "ofp/yaml/ydatapathid.h"
#include "ofp/driver.h"
#include "ofp/padding.h"
#include "ofp/yaml/ybytelist.h"
#include "ofp/yaml/yaddress.h"
#include "ofp/yaml/encoder.h"
#include "ofp/yaml/ytimestamp.h"
#include "ofp/rpc/rpcid.h"

namespace ofp {
namespace rpc {

/// The maximum RPC message size is 1MB.
const size_t RPC_MAX_MESSAGE_SIZE = 1048576;

/// RPC Methods
enum RpcMethod : UInt32 {
  METHOD_LISTEN = 0,    // OFP.LISTEN
  METHOD_CONNECT,       // OFP.CONNECT
  METHOD_CLOSE,         // OFP.CLOSE
  METHOD_SEND,          // OFP.SEND
  METHOD_CHANNEL,       // OFP.CHANNEL
  METHOD_MESSAGE,       // OFP.MESSAGE
  METHOD_ALERT,         // OFP.ALERT
  METHOD_LIST_CONNS,    // OFP.LIST_CONNECTIONS
  METHOD_ADD_IDENTITY,  // OFP.ADD_IDENTITY
  METHOD_DESCRIPTION,   // OFP.DESCRIPTION
  METHOD_UNSUPPORTED
};

/// JSON-RPC error codes.
enum RpcErrorCode {
  ERROR_CODE_INVALID_REQUEST = -32600,
  ERROR_CODE_METHOD_NOT_FOUND = -32601,
  ERROR_CODE_CONNECTION_NOT_FOUND = -65000,
  ERROR_CODE_TLS_ID_NOT_FOUND = -65001,
  ERROR_CODE_INVALID_OPTIONS = -65002,
};

OFP_BEGIN_IGNORE_PADDING

/// Represents the general JSON-RPC error response.
struct RpcErrorResponse {
  explicit RpcErrorResponse(RpcID ident) : id{ident} {}
  std::string toJson();

  struct Error {
    SignedInt32 code = 0;
    std::string message;
  };

  RpcID id;
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

/// Represents a RPC request to describe the RPC server (METHOD_DESCRIPTION)
struct RpcDescription {
  explicit RpcDescription(RpcID ident) : id{ident} {}

  struct Params {};

  RpcID id;
  Params params;
};

/// Represents a RPC response to describe the RPC server (METHOD_DESCRIPTION)
struct RpcDescriptionResponse {
  explicit RpcDescriptionResponse(RpcID ident) : id{ident} {}
  std::string toJson();

  struct Result {
    /// Current API version.
    UInt16 major_version;
    UInt16 minor_version;
    /// Current version of this software.
    std::string software_version;
    /// List of supported OpenFlow versions.
    std::vector<UInt8> ofp_versions;
  };

  RpcID id;
  Result result;
};

/// Represents a RPC request to listen for new connections (METHOD_LISTEN)
struct RpcListen {
  explicit RpcListen(RpcID ident) : id{ident} {}

  struct Params {
    /// Endpoint to listen on.
    IPv6Endpoint endpoint;
    /// Optional list of desired versions (empty means all).
    std::vector<UInt8> versions;
    /// Optional TLS identity.
    UInt64 securityId = 0;
    /// Array of options.
    std::vector<std::string> options;
  };

  RpcID id;
  Params params;
};

/// Represents a RPC response to listen for new connections (METHOD_LISTEN)
struct RpcListenResponse {
  explicit RpcListenResponse(RpcID ident) : id{ident} {}
  std::string toJson();

  struct Result {
    /// Connection ID of listening connection.
    UInt64 connId = 0;
  };

  RpcID id;
  Result result;
};

/// Represents a RPC request to make an outgoing connection (METHOD_CONNECT)
struct RpcConnect {
  explicit RpcConnect(RpcID ident) : id{ident} {}

  struct Params {
    /// Endpoint to connect to.
    IPv6Endpoint endpoint;
    /// Optional list of desired versions (empty means all).
    std::vector<UInt8> versions;
    /// Optional TLS identity.
    UInt64 securityId = 0;
    /// Array of options.
    std::vector<std::string> options;
  };

  RpcID id;
  Params params;
};

/// Represents a RPC response to make an outgoing connection (METHOD_CONNECT)
struct RpcConnectResponse {
  explicit RpcConnectResponse(RpcID ident) : id{ident} {}
  std::string toJson();

  struct Result {
    /// Connection ID of outgoing connection.
    UInt64 connId = 0;
  };

  RpcID id;
  Result result;
};

/// Represents a RPC request to close a connection (METHOD_CLOSE)
struct RpcClose {
  explicit RpcClose(RpcID ident) : id{ident} {}

  struct Params {
    /// Connection ID to close.
    UInt64 connId = 0;
  };

  RpcID id;
  Params params;
};

/// Represents a RPC response to close a connection (METHOD_CLOSE)
struct RpcCloseResponse {
  explicit RpcCloseResponse(RpcID ident) : id{ident} {}
  std::string toJson();

  struct Result {
    /// Count of closed connections.
    UInt32 count = 0;
  };

  RpcID id;
  Result result;
};

/// Represents a RPC request to list connection stats (METHOD_LIST_CONNS)
struct RpcListConns {
  explicit RpcListConns(RpcID ident) : id{ident} {}

  struct Params {
    UInt64 connId = 0;
  };

  RpcID id;
  Params params;
};

struct RpcListConnsResponse {
  explicit RpcListConnsResponse(RpcID ident) : id{ident} {}
  std::string toJson();

  using Result = std::vector<RpcConnectionStats>;

  RpcID id;
  Result result;
};

/// Represents a RPC request to add an identity (METHOD_ADD_IDENTITY)
struct RpcAddIdentity {
  explicit RpcAddIdentity(RpcID ident) : id{ident} {}

  struct Params {
    /// Certificate chain in PEM format (which also contains private key).
    std::string cert;
    /// PEM certificate for trusted CA to use for verifying a peer certificate.
    std::string cert_auth;
    /// Optional password for encrypted private key.
    std::string privkey_password;
  };

  RpcID id;
  Params params;
};

/// Represents a RPC response to add an identity (METHOD_ADD_IDENTITY)
struct RpcAddIdentityResponse {
  explicit RpcAddIdentityResponse(RpcID ident) : id{ident} {}
  std::string toJson();

  struct Result {
    /// Security ID of identity added.
    UInt64 securityId = 0;
  };

  RpcID id;
  Result result;
};

/// Represents a RPC request to send a message to datapath (METHOD_SEND).
struct RpcSend {
  explicit RpcSend(RpcID ident, yaml::Encoder::ChannelFinder finder)
      : id{ident}, params{finder} {}

  RpcID id;
  yaml::Encoder params;
};

/// Represnts a RPC response to send a message to datapath (METHOD_SEND).
struct RpcSendResponse {
  explicit RpcSendResponse(RpcID ident) : id{ident} {}
  std::string toJson();

  struct Result {
    ByteRange data;  // header of message sent
  };

  RpcID id;
  Result result;
};

/// Represents a RPC notification about a channel (METHOD_CHANNEL)
struct RpcChannel {
  std::string toJson();

  struct Params {
    UInt64 connId = 0;
    DatapathID datapathId;
    std::string status;
    IPv6Endpoint endpoint;
    UInt8 version{};
  };

  Params params;
};

/// Represents a RPC notification about some alert (e.g. malformed message)
/// (METHOD_ALERT).
struct RpcAlert {
  std::string toJson();

  struct Params {
    UInt64 connId = 0;
    DatapathID datapathId;
    Timestamp time;
    UInt32 xid = 0;
    std::string alert;
    ByteRange data;
  };

  Params params;
};

OFP_END_IGNORE_PADDING

}  // namespace rpc
}  // namespace ofp

LLVM_YAML_IS_SEQUENCE_VECTOR(ofp::rpc::RpcConnectionStats)

namespace llvm {
namespace yaml {

const char *const kRpcSchema = R"""(
{Rpc/OFP.DESCRIPTION}
id: UInt64
method: !request OFP.DESCRIPTION
result: !reply
  major_version: UInt16
  minor_version: UInt16
  software_version: String
  ofp_versions: [UInt8]

{Rpc/OFP.LISTEN}
id: !opt UInt64
method: !request OFP.LISTEN
params: !request
  endpoint: IPv6Endpoint
  versions: !opt [UInt8]
  tls_id: !opt UInt64
  options: !opt [String]
result: !reply
  conn_id: UInt64

{Rpc/OFP.CONNECT}
id: !opt UInt64
method: !request OFP.CONNECT
params: !request
  endpoint: IPv6Endpoint
  versions: !opt [UInt8]
  tls_id: !opt UInt64
  options: !opt [String]
result: !reply
  conn_id: UInt64

{Rpc/OFP.CLOSE}
id: !opt UInt64
method: !request OFP.CLOSE
params: !request
  conn_id: UInt64
result: !reply
  count: UInt32

{Rpc/OFP.SEND}
id: !opt UInt64
method: !request OFP.SEND
params: !request Message
result: !reply
  data: HexData

{Rpc/OFP.LIST_CONNECTIONS}
id: !opt UInt64
method: !request OFP.LIST_CONNECTIONS
params: !request
  conn_id: UInt64
result: !reply
  - local_endpoint: IPv6Endpoint
    remote_endpoint: IPv6Endpoint
    datapath_id: DatapathID
    conn_id: UInt64
    auxiliary_id: UInt8
    transport: TCP | UDP | TLS | DTLS | NONE

{Rpc/OFP.ADD_IDENTITY}
id: !opt UInt64
method: !request OFP.ADD_IDENTITY
params: !request
  cert: String
  cert_auth: String
  privkey_password: !opt String
result: !reply
  tls_id: UInt64

{Rpc/OFP.MESSAGE}
method: !notify OFP.MESSAGE
params: !notify Message

{Rpc/OFP.CHANNEL}
method: !notify OFP.CHANNEL
params: !notify
  conn_id: UInt64
  datapath_id: DatapathID
  endpoint: IPv6Endpoint
  version: UInt8
  status: UP | DOWN

{Rpc/OFP.ALERT}
method: !notify OFP.ALERT
params: !notify
  conn_id: UInt64
  datapath_id: DatapathID
  xid: UInt32
  time: Timestamp
  alert: String
  data: HexData

{Rpc/rpc.error}
id: UInt64
error:
  code: SInt32
  message: String
)""";

template <>
struct ScalarTraits<ofp::rpc::RpcMethod> {
  static const ofp::yaml::EnumConverter<ofp::rpc::RpcMethod> converter;

  static void output(const ofp::rpc::RpcMethod &value, void *ctxt,
                     llvm::raw_ostream &out) {
    llvm::StringRef scalar;
    if (converter.convert(value, &scalar)) {
      out << scalar;
    } else {
      out << format("0x%02X", value);
    }
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::rpc::RpcMethod &value) {
    if (!converter.convert(scalar, &value)) {
      value = ofp::rpc::METHOD_UNSUPPORTED;
      return "unknown method";
    }

    return "";
  }

  static bool mustQuote(StringRef) { return false; }
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
struct MappingTraits<ofp::rpc::RpcDescription::Params> {
  static void mapping(IO &io, ofp::rpc::RpcDescription::Params &params) {}
};

template <>
struct MappingTraits<ofp::rpc::RpcListen::Params> {
  static void mapping(IO &io, ofp::rpc::RpcListen::Params &params) {
    io.mapRequired("endpoint", params.endpoint);
    io.mapOptional("versions", params.versions);
    io.mapOptional("tls_id", params.securityId);
    io.mapOptional("options", params.options);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcConnect::Params> {
  static void mapping(IO &io, ofp::rpc::RpcConnect::Params &params) {
    io.mapRequired("endpoint", params.endpoint);
    io.mapOptional("versions", params.versions);
    io.mapOptional("tls_id", params.securityId);
    io.mapOptional("options", params.options);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcClose::Params> {
  static void mapping(IO &io, ofp::rpc::RpcClose::Params &params) {
    io.mapRequired("conn_id", params.connId);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcListConns::Params> {
  static void mapping(IO &io, ofp::rpc::RpcListConns::Params &result) {
    io.mapRequired("conn_id", result.connId);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcAddIdentity::Params> {
  static void mapping(IO &io, ofp::rpc::RpcAddIdentity::Params &params) {
    io.mapRequired("cert", params.cert);
    io.mapRequired("cert_auth", params.cert_auth);
    io.mapOptional("privkey_password", params.privkey_password);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcDescriptionResponse> {
  static void mapping(IO &io, ofp::rpc::RpcDescriptionResponse &response) {
    io.mapRequired("id", response.id);
    io.mapRequired("result", response.result);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcDescriptionResponse::Result> {
  static void mapping(IO &io,
                      ofp::rpc::RpcDescriptionResponse::Result &result) {
    io.mapRequired("major_version", result.major_version);
    io.mapRequired("minor_version", result.minor_version);
    io.mapRequired("software_version", result.software_version);
    io.mapRequired("ofp_versions", result.ofp_versions);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcListenResponse> {
  static void mapping(IO &io, ofp::rpc::RpcListenResponse &response) {
    io.mapRequired("id", response.id);
    io.mapRequired("result", response.result);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcListenResponse::Result> {
  static void mapping(IO &io, ofp::rpc::RpcListenResponse::Result &result) {
    io.mapRequired("conn_id", result.connId);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcConnectResponse> {
  static void mapping(IO &io, ofp::rpc::RpcConnectResponse &response) {
    io.mapRequired("id", response.id);
    io.mapRequired("result", response.result);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcConnectResponse::Result> {
  static void mapping(IO &io, ofp::rpc::RpcConnectResponse::Result &result) {
    io.mapRequired("conn_id", result.connId);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcCloseResponse> {
  static void mapping(IO &io, ofp::rpc::RpcCloseResponse &response) {
    io.mapRequired("id", response.id);
    io.mapRequired("result", response.result);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcCloseResponse::Result> {
  static void mapping(IO &io, ofp::rpc::RpcCloseResponse::Result &result) {
    io.mapRequired("count", result.count);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcListConnsResponse> {
  static void mapping(IO &io, ofp::rpc::RpcListConnsResponse &response) {
    io.mapRequired("id", response.id);
    io.mapRequired("result", response.result);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcConnectionStats> {
  static void mapping(IO &io, ofp::rpc::RpcConnectionStats &stats) {
    io.mapRequired("local_endpoint", stats.localEndpoint);
    io.mapRequired("remote_endpoint", stats.remoteEndpoint);
    io.mapRequired("datapath_id", stats.datapathId);
    io.mapRequired("conn_id", stats.connId);
    io.mapRequired("auxiliary_id", stats.auxiliaryId);
    io.mapRequired("transport", stats.transport);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcAddIdentityResponse> {
  static void mapping(IO &io, ofp::rpc::RpcAddIdentityResponse &response) {
    io.mapRequired("id", response.id);
    io.mapRequired("result", response.result);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcAddIdentityResponse::Result> {
  static void mapping(IO &io,
                      ofp::rpc::RpcAddIdentityResponse::Result &result) {
    io.mapRequired("tls_id", result.securityId);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcSendResponse> {
  static void mapping(IO &io, ofp::rpc::RpcSendResponse &response) {
    io.mapRequired("id", response.id);
    io.mapRequired("result", response.result);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcSendResponse::Result> {
  static void mapping(IO &io, ofp::rpc::RpcSendResponse::Result &result) {
    io.mapRequired("data", result.data);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcErrorResponse> {
  static void mapping(IO &io, ofp::rpc::RpcErrorResponse &response) {
    io.mapRequired("id", response.id);
    io.mapRequired("error", response.error);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcErrorResponse::Error> {
  static void mapping(IO &io, ofp::rpc::RpcErrorResponse::Error &error) {
    io.mapRequired("code", error.code);
    io.mapRequired("message", error.message);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcChannel> {
  static void mapping(IO &io, ofp::rpc::RpcChannel &response) {
    ofp::rpc::RpcMethod method = ofp::rpc::METHOD_CHANNEL;
    io.mapRequired("method", method);
    io.mapRequired("params", response.params);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcChannel::Params> {
  static void mapping(IO &io, ofp::rpc::RpcChannel::Params &params) {
    io.mapRequired("conn_id", params.connId);
    io.mapOptional("datapath_id", params.datapathId, ofp::DatapathID{});
    io.mapRequired("endpoint", params.endpoint);
    io.mapRequired("version", params.version);
    io.mapRequired("status", params.status);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcAlert> {
  static void mapping(IO &io, ofp::rpc::RpcAlert &response) {
    ofp::rpc::RpcMethod method = ofp::rpc::METHOD_ALERT;
    io.mapRequired("method", method);
    io.mapRequired("params", response.params);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcAlert::Params> {
  static void mapping(IO &io, ofp::rpc::RpcAlert::Params &params) {
    io.mapRequired("conn_id", params.connId);
    io.mapRequired("datapath_id", params.datapathId);
    io.mapRequired("xid", params.xid);
    io.mapRequired("time", params.time);
    io.mapRequired("alert", params.alert);
    io.mapRequired("data", params.data);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_RPC_RPCEVENTS_H_
