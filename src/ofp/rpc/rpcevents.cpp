// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/rpc/rpcevents.h"
#include "ofp/yaml/outputjson.h"

namespace {

/// Converts event to a JSON representation, terminated by line feed.
template <class Type>
std::string toJsonString(Type *event) {
  std::string json;
  llvm::raw_string_ostream rss(json);
  ofp::yaml::OutputJson yout{rss};
  yout << *event;
  rss << '\n';
  return rss.str();
}

}  // namespace

using namespace ofp::rpc;

std::string RpcListenResponse::toJson() { return toJsonString(this); }

std::string RpcConnectResponse::toJson() { return toJsonString(this); }

std::string RpcCloseResponse::toJson() { return toJsonString(this); }

std::string RpcSendResponse::toJson() { return toJsonString(this); }

std::string RpcErrorResponse::toJson() { return toJsonString(this); }

std::string RpcDatapath::toJson() { return toJsonString(this); }

std::string RpcMessageError::toJson() { return toJsonString(this); }

std::string RpcListConnsResponse::toJson() { return toJsonString(this); }

std::string RpcAddIdentityResponse::toJson() { return toJsonString(this); }

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

// N.B. These strings must be in same order as RpcMethod enum.
static llvm::StringRef sRpcMethods[] = {
    "ofp.listen", "ofp.connect", "ofp.close", "ofp.send", "ofp.datapath",
    "ofp.message", "ofp.message_error", "ofp.list_connections",
    "ofp.add_identity"};

ofp::yaml::EnumConverter<ofp::rpc::RpcMethod>
    llvm::yaml::ScalarTraits<ofp::rpc::RpcMethod>::converter{sRpcMethods};

OFP_END_IGNORE_GLOBAL_CONSTRUCTOR
