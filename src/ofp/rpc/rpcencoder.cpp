// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/rpc/rpcencoder.h"
#include "ofp/rpc/rpcconnection.h"
#include "ofp/yaml/seterror.h"

using namespace ofp;
using namespace ofp::rpc;

inline bool errorFound(llvm::yaml::IO &io) {
  return ofp::yaml::ErrorFound(io);
}

RpcEncoder::RpcEncoder(const std::string &input, RpcConnection *conn,
                       yaml::Encoder::ChannelFinder finder)
    : conn_{conn}, errorStream_{error_}, finder_{finder} {
  llvm::yaml::Input yin{input, nullptr, RpcEncoder::diagnosticHandler, this};
  if (!yin.error()) {
    yin >> *this;
  }

  if (yin.error() && method_ != METHOD_SEND) {
    replyError();
  }
}

void RpcEncoder::diagnosticHandler(const llvm::SMDiagnostic &diag,
                                   void *context) {
  RpcEncoder *encoder = reinterpret_cast<RpcEncoder *>(context);
  encoder->addDiagnostic(diag);
}

void RpcEncoder::addDiagnostic(const llvm::SMDiagnostic &diag) {
  diag.print("", errorStream_, false);
}

void RpcEncoder::encodeParams(llvm::yaml::IO &io) {
  // Check jsonrpc_ value, if provided.
  if (!jsonrpc_.empty() && jsonrpc_ != "2.0") {
    io.setError("Unsupported jsonrpc version");
    return;
  }

  switch (method_) {
    case METHOD_LISTEN: {
      RpcListen listen{id_};
      io.mapRequired("params", listen.params);
      if (!errorFound(io)) {
        conn_->onRpcListen(&listen);
      }
      break;
    }
    case METHOD_CONNECT: {
      RpcConnect connect{id_};
      io.mapRequired("params", connect.params);
      if (!errorFound(io)) {
        conn_->onRpcConnect(&connect);
      }
      break;
    }
    case METHOD_CLOSE: {
      RpcClose close{id_};
      io.mapRequired("params", close.params);
      if (!errorFound(io)) {
        conn_->onRpcClose(&close);
      }
      break;
    }
    case METHOD_SEND: {
      void *savedContext = io.getContext();
      RpcSend send{id_, finder_};
      yaml::detail::YamlContext ctxt{&send.params, &io};
      io.setContext(&ctxt);
      io.mapRequired("params", send.params);
      io.setContext(savedContext);
      if (!errorFound(io)) {
        conn_->onRpcSend(&send);
      } else {
        replySendError(send);
      }
      break;
    }
    case METHOD_LIST_CONNS: {
      RpcListConns list{id_};
      io.mapRequired("params", list.params);
      if (!errorFound(io)) {
        conn_->onRpcListConns(&list);
      }
      break;
    }
    case METHOD_ADD_IDENTITY: {
      RpcAddIdentity add{id_};
      io.mapRequired("params", add.params);
      if (!errorFound(io)) {
        conn_->onRpcAddIdentity(&add);
      }
      break;
    }
    case METHOD_MESSAGE:
      io.setError("Use 'OFP.SEND'. 'OFP.MESSAGE' is for notifications only");
      break;
    case METHOD_DESCRIPTION: {
      RpcDescription desc{id_};
      io.mapOptional("params", desc.params);
      if (!errorFound(io)) {
        conn_->onRpcDescription(&desc);
      }
      break;
    }
    case METHOD_SET_FILTER: {
      RpcSetFilter set{id_};
      io.mapRequired("params", set.params);
      if (!errorFound(io)) {
        conn_->onRpcSetFilter(&set);
      }
      break;
    }
    default:
      break;
  }
}

void RpcEncoder::replyError() {
  // Error string will be empty if there's no content.

  RpcErrorResponse response{id_.is_missing() ? RpcID::NULL_VALUE : id_};
  response.error.message = llvm::StringRef{error()}.rtrim();

  // Handle case where the generated error response is too big to send back.
  // Use max rpc message size minus some slop. When message is truncated, tack
  // on '...(truncated)'
  const size_t kMaxMessageLen = RPC_MAX_MESSAGE_SIZE - 500;
  size_t messageLen = response.error.message.length();
  if (messageLen > kMaxMessageLen) {
    response.error.message.resize(kMaxMessageLen);
    response.error.message += "...(truncated)";
  }

  int code = ERROR_CODE_INVALID_REQUEST;
  if (response.error.message.find("unknown method") != std::string::npos) {
    code = ERROR_CODE_METHOD_NOT_FOUND;
  } else if (response.error.message.find("unable to locate") !=
             std::string::npos) {
    code = ERROR_CODE_CONNECTION_NOT_FOUND;
  }

  response.error.code = code;
  log_warning("JSON-RPC error:", response.error.message);

  if (conn_) {
    // conn_ is set to null in one of the unit tests. Under normal conditions,
    // conn_ should never be null.
    conn_->rpcReply(&response);
  }
}

void RpcEncoder::replySendError(const RpcSend &send) {
  if (!id_.is_missing()) {
    replyError();
  } else if (conn_) {
    // Send CHANNEL_ALERT to report failure to send message, unless NO_ALERT
    // flag is set.
    OFPMessageFlags flags = send.params.flags();
    if ((flags & OFP_NO_ALERT) == 0) {
      llvm::StringRef errmsg = llvm::StringRef{error()}.rtrim();
      log_warning("Failed to send message:", errmsg);
      UInt32 xid = send.params.xid();
      UInt64 connId = send.params.connectionId();
      DatapathID dpid = send.params.datapathId();
      conn_->rpcAlert(dpid, connId, errmsg, {}, Timestamp::now(), xid);
    }
  }
}
