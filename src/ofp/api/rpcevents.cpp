#include "ofp/api/rpcevents.h"
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

using namespace ofp::api;

std::string RpcOpenResponse::toJson() {
  return toJsonString(this);
}

std::string RpcSendResponse::toJson() {
  return toJsonString(this);
}


std::string RpcErrorResponse::toJson() {
  return toJsonString(this);
}


std::string RpcDatapath::toJson() {
  return toJsonString(this);
}
