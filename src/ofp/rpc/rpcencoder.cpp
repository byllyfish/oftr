// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

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
  // FIXME: Checking for double-quote should ignore preceding white space.
  std::string rawInput;
  if (!input.empty() && input[0] == '"') {
    llvm::yaml::Input ys{input, nullptr, RpcEncoder::diagnosticHandler, this};
    if (!ys.error()) {
      ys >> rawInput;
    }
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
        replySendError();
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
    case METHOD_CHANNEL:
      io.setError("'OFP.CHANNEL' is for notifications only");
      break;
    case METHOD_MESSAGE:
      io.setError("Use 'OFP.SEND'. 'OFP.MESSAGE' is for notifications only");
      break;
    case METHOD_ALERT:
      io.setError("'OFP.ALERT' is for notifications only");
      break;
    case METHOD_DESCRIPTION: {
      RpcDescription desc{id_};
      io.mapOptional("params", desc.params);
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
  log::warning("JSON-RPC error:", response.error.message);

  if (conn_) {
    // conn_ is set to null in one of the unit tests. Under normal conditions,
    // conn_ should never be null.
    conn_->rpcReply(&response);
  }
}

void RpcEncoder::replySendError() {
  if (!id_.is_missing()) {
    replyError();
  } else {
    // Send OFP.ALERT to report failure to send message.

    RpcAlert notification;
    notification.params.time = Timestamp::now();
    notification.params.alert = llvm::StringRef{error()}.rtrim();
    if (conn_) {
      conn_->rpcReply(&notification);
    }
  }
}
