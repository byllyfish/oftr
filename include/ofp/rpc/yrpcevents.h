// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_RPC_YRPCEVENTS_H_
#define OFP_RPC_YRPCEVENTS_H_

#include "ofp/rpc/rpcevents.h"
#include "ofp/yaml/yaddress.h"
#include "ofp/yaml/ybytelist.h"
#include "ofp/yaml/ydatapathid.h"
#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/yratelimiter.h"
#include "ofp/yaml/ytimestamp.h"

LLVM_YAML_IS_SEQUENCE_VECTOR(ofp::rpc::RpcConnectionStats)
LLVM_YAML_IS_SEQUENCE_VECTOR(ofp::rpc::FilterTableEntry)

namespace llvm {
namespace yaml {

const char *const kRpcSchema = R"""(
{Rpc/OFP.DESCRIPTION}
id: UInt64
method: !request OFP.DESCRIPTION
result: !reply
  api_version: String
  sw_desc: String
  versions: [UInt8]

{Rpc/OFP.LISTEN}
id: !opt UInt64
method: !request OFP.LISTEN
params: !request
  endpoint: IPEndpoint
  versions: !opt [UInt8]
  tls_id: !opt UInt64
  options: !opt [String]
result: !reply
  conn_id: UInt64

{Rpc/OFP.CONNECT}
id: !opt UInt64
method: !request OFP.CONNECT
params: !request
  endpoint: IPEndpoint
  versions: !opt [UInt8]
  tls_id: !opt UInt64
  options: !opt [String]
result: !reply
  conn_id: UInt64

{Rpc/OFP.CLOSE}
id: !opt UInt64
method: !request OFP.CLOSE
params: !request
  conn_id: !opt UInt64
  datapath_id: !opt DatapathID
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
  stats:
    - local_endpoint: IPEndpoint
      remote_endpoint: IPEndpoint
      datapath_id: DatapathID
      conn_id: UInt64
      auxiliary_id: UInt8
      transport: TCP | UDP | TLS | DTLS | NONE

{Rpc/OFP.ADD_IDENTITY}
id: !opt UInt64
method: !request OFP.ADD_IDENTITY
params: !request
  cert: String
  cacert: String
  privkey: String
  version: !opt String
  ciphers: !opt String
result: !reply
  tls_id: UInt64

{Rpc/OFP.MESSAGE}
method: !notify OFP.MESSAGE
params: !notify Message

{Rpc/OFP.MESSAGE}
method: !notify OFP.MESSAGE
params: !notify
  type: CHANNEL_UP | CHANNEL_DOWN
  time: Timestamp
  conn_id: UInt64
  datapath_id: !optout DatapathID
  version: UInt8
  endpoint: IPEndpoint

{Rpc/OFP.MESSAGE}
method: !notify OFP.MESSAGE
params: !notify
  type: CHANNEL_ALERT
  time: Timestamp
  conn_id: UInt64
  datapath_id: !optout DatapathID
  xid: UInt32
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
    io.mapOptional("conn_id", params.connId);
    io.mapOptional("datapath_id", params.datapathId);
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
    io.mapRequired("cacert", params.cacert);
    io.mapRequired("privkey", params.privkey);
    io.mapOptional("version", params.version);
    io.mapOptional("ciphers", params.ciphers);
  }
};

template <>
struct MappingTraits<ofp::rpc::FilterTableEntry> {
  static void mapping(IO &io, ofp::rpc::FilterTableEntry &entry) {
    using namespace ofp::rpc;

    std::string filter;
    io.mapRequired("filter", filter);
    if (!entry.setFilter(filter)) {
      io.setError("invalid pcap filter");
    }

    FilterAction::Type actionType = FilterAction::NONE;
    io.mapRequired("action", actionType);

    switch (actionType) {
      case FilterAction::GENERIC_REPLY: {
        auto action = ofp::MakeUniquePtr<FilterActionGenericReply>();
        // io.mapRequired("params", *action);
        entry.setAction(std::move(action));
        break;
      }
      case FilterAction::NONE:
        break;
    }

    RateLimiter rateLimiter{false};
    io.mapOptional("escalate", rateLimiter);
    entry.setEscalate(rateLimiter);
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
    io.mapRequired("api_version", result.api_version);
    io.mapRequired("sw_desc", result.sw_desc);
    io.mapRequired("versions", result.versions);
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
struct MappingTraits<ofp::rpc::RpcListConnsResponse::Result> {
  static void mapping(IO &io, ofp::rpc::RpcListConnsResponse::Result &result) {
    io.mapRequired("stats", result.stats);
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
struct MappingTraits<ofp::rpc::RpcSetFilterResponse> {
  static void mapping(IO &io, ofp::rpc::RpcSetFilterResponse &response) {
    io.mapRequired("id", response.id);
    io.mapRequired("result", response.result);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcSetFilterResponse::Result> {
  static void mapping(IO &io, ofp::rpc::RpcSetFilterResponse::Result &result) {
    io.mapRequired("count", result.count);
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
    ofp::rpc::RpcMethod method = ofp::rpc::METHOD_MESSAGE;
    io.mapRequired("method", method);
    io.mapRequired("params", response.params);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcChannel::Params> {
  static void mapping(IO &io, ofp::rpc::RpcChannel::Params &params) {
    io.mapRequired("type", params.type);
    io.mapRequired("time", params.time);
    io.mapRequired("conn_id", params.connId);
    io.mapOptional("datapath_id", params.datapathId, ofp::DatapathID{});
    io.mapRequired("endpoint", params.endpoint);
    io.mapRequired("version", params.version);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcAlert> {
  static void mapping(IO &io, ofp::rpc::RpcAlert &response) {
    ofp::rpc::RpcMethod method = ofp::rpc::METHOD_MESSAGE;
    io.mapRequired("method", method);
    io.mapRequired("params", response.params);
  }
};

template <>
struct MappingTraits<ofp::rpc::RpcAlert::Params> {
  static void mapping(IO &io, ofp::rpc::RpcAlert::Params &params) {
    io.mapRequired("type", params.type);
    io.mapRequired("time", params.time);
    io.mapRequired("conn_id", params.connId);
    io.mapOptional("datapath_id", params.datapathId, ofp::DatapathID{});
    io.mapRequired("xid", params.xid);
    io.mapRequired("alert", params.alert);
    io.mapRequired("data", params.data);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_RPC_YRPCEVENTS_H_
