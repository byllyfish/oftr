// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/rpc/rpcencoder.h"
#include "ofp/rpc/rpcconnection.h"

using ofp::rpc::RpcEncoder;

static bool errorFound(llvm::yaml::IO &io) {
  // This is a kludge. We need to know if the io object encountered an error
  // but the IO class doesn't support this. We need to reach into the Input
  // subclass to check for the error.
  llvm::yaml::Input *yin = static_cast<llvm::yaml::Input *>(&io);
  return static_cast<bool>(yin->error());
}

RpcEncoder::RpcEncoder(const std::string &input, RpcConnection *conn,
                       yaml::Encoder::ChannelFinder finder)
    : conn_{conn}, errorStream_{error_}, finder_{finder} {
  // Check if input string is possibly a JSON/YAML quoted string.
  std::string rawInput;
  if (!input.empty() && input[0] == '"') {
    log::debug("RpcEncoder: decodeString:", input);
    llvm::yaml::Input ys{input, nullptr, RpcEncoder::diagnosticHandler, this};
    if (!ys.error()) {
      ys >> rawInput;
    }
    log::debug("RpcEncoder: result:", rawInput);
    if (ys.error()) {
      replyError();
      return;
    }
  }

  // If there's any rawInput, use that value. Otherwise decode the original
  // input.

  const std::string &inputText = rawInput.empty() ? input : rawInput;

  llvm::yaml::Input yin{inputText, nullptr, RpcEncoder::diagnosticHandler,
                        this};
  if (!yin.error()) {
    yin >> *this;
  }

  if (yin.error()) {
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
  // Make sure no one uses an explicit ID value of 2^64 - 1. We use this value
  // to indicate a missing ID value. (FIXME)
  UInt64 id;
  if (id_) {
    id = *id_;
    if (id == RPC_ID_MISSING) {
      method_ = METHOD_UNSUPPORTED;
      return;
    }
  } else {
    id = RPC_ID_MISSING;
  }

  switch (method_) {
    case METHOD_LISTEN: {
      RpcListen listen{id};
      io.mapRequired("params", listen.params);
      if (!errorFound(io)) {
        conn_->onRpcListen(&listen);
      }
      break;
    }
    case METHOD_CONNECT: {
      RpcConnect connect{id};
      io.mapRequired("params", connect.params);
      if (!errorFound(io)) {
        conn_->onRpcConnect(&connect);
      }
      break;
    }
    case METHOD_CLOSE: {
      RpcClose close{id};
      io.mapRequired("params", close.params);
      if (!errorFound(io)) {
        conn_->onRpcClose(&close);
      }
      break;
    }
    case METHOD_SEND: {
      void *savedContext = io.getContext();
      RpcSend send{id, finder_};
      yaml::detail::YamlContext ctxt{&send.params};
      io.setContext(&ctxt);
      io.mapRequired("params", send.params);
      io.setContext(savedContext);
      if (!errorFound(io)) {
        conn_->onRpcSend(&send);
      }
      break;
    }
    case METHOD_LIST_CONNS: {
      RpcListConns list{id};
      io.mapRequired("params", list.params);
      if (!errorFound(io)) {
        conn_->onRpcListConns(&list);
      }
      break;
    }
    case METHOD_ADD_IDENTITY: {
      RpcAddIdentity add{id};
      io.mapRequired("params", add.params);
      if (!errorFound(io)) {
        conn_->onRpcAddIdentity(&add);
      }
      break;
    }
    case METHOD_CHANNEL:
      io.setError("'ofp.channel' is for notifications only.");
      break;
    case METHOD_MESSAGE:
      io.setError(
          "Use 'ofp.send' instead; 'ofp.message' is for notifications only.");
      break;
    case METHOD_ALERT:
      io.setError("'ofp.alert' is for notifications only.");
      break;
    case METHOD_DESCRIPTION: {
      RpcDescription desc{id};
      if (!errorFound(io)) {
        conn_->onRpcDescription(&desc);
      }
      break;
    }
    default:
      break;
  }
}

void RpcEncoder::replyError() {
  // Error string will be empty if there's no content.

  RpcErrorResponse response{id_ ? *id_ : 0};
  response.error.message = llvm::StringRef{error()}.rtrim();

  int code = ERROR_CODE_INVALID_REQUEST;
  if (response.error.message.find("unknown method") != std::string::npos) {
    code = ERROR_CODE_METHOD_NOT_FOUND;
  }

  response.error.code = code;
  conn_->rpcReply(&response);

  log::warning("JSON-RPC parse error:", response.error.message);
}
