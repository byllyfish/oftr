// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

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

std::string RpcDescriptionResponse::toJson() {
  return toJsonString(this);
}

std::string RpcListenResponse::toJson() {
  return toJsonString(this);
}

std::string RpcConnectResponse::toJson() {
  return toJsonString(this);
}

std::string RpcCloseResponse::toJson() {
  return toJsonString(this);
}

std::string RpcSendResponse::toJson() {
  return toJsonString(this);
}

std::string RpcErrorResponse::toJson() {
  return toJsonString(this);
}

std::string RpcChannel::toJson() {
  return toJsonString(this);
}

std::string RpcAlert::toJson() {
  return toJsonString(this);
}

std::string RpcListConnsResponse::toJson() {
  return toJsonString(this);
}

std::string RpcAddIdentityResponse::toJson() {
  return toJsonString(this);
}

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

// N.B. These strings must be in same order as RpcMethod enum.
static const llvm::StringRef sRpcMethods[] = {
    "OFP.LISTEN",       "OFP.CONNECT",    "OFP.CLOSE", "OFP.SEND",
    "OFP.CHANNEL",      "OFP.MESSAGE",    "OFP.ALERT", "OFP.LIST_CONNECTIONS",
    "OFP.ADD_IDENTITY", "OFP.DESCRIPTION"};

const ofp::yaml::EnumConverter<ofp::rpc::RpcMethod>
    llvm::yaml::ScalarTraits<ofp::rpc::RpcMethod>::converter{sRpcMethods};

OFP_END_IGNORE_GLOBAL_CONSTRUCTOR
