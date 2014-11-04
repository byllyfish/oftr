// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/api/apiencoder.h"
#include "ofp/api/apiconnection.h"

using ofp::api::ApiEncoder;
using YamlInput = llvm::yaml::Input;

#if 0

static bool errorFound(llvm::yaml::IO &io) {
  // This is a kludge. We need to know if the io object encountered an error
  // but the IO class doesn't support this. We need to reach into the Input
  // subclass to check for the error.
  llvm::yaml::Input *yin = static_cast<llvm::yaml::Input *>(&io);
  return yin->error();
}

ApiEncoder::ApiEncoder(const std::string &input, ApiConnection *conn)
    : conn_{conn}, errorStream_{error_} {
  if (!input.empty()) {
    YamlInput yin{input, nullptr, ApiEncoder::diagnosticHandler, this};
    if (!yin.error()) {
      yin >> *this;
    }
    if (yin.error()) {
      conn_->onYamlError(errorStream_.str(), input);
    }
  }
}

void ApiEncoder::diagnosticHandler(const llvm::SMDiagnostic &diag,
                                   void *context) {
  ApiEncoder *encoder = reinterpret_cast<ApiEncoder *>(context);
  encoder->addDiagnostic(diag);
}

void ApiEncoder::encodeMsg(llvm::yaml::IO &io, ApiEvent event) {
  switch (event) {
    case LIBOFP_LOOPBACK: {
      ApiLoopback loopback;
      io.mapRequired("params", loopback.params);
      if (!errorFound(io)) conn_->onLoopback(&loopback);
      break;
    }
    case LIBOFP_LISTEN_REQUEST: {
      ApiListenRequest listenReq;
      io.mapRequired("params", listenReq.params);
      if (!errorFound(io)) conn_->onListenRequest(&listenReq);
      break;
    }
    case LIBOFP_CONNECT_REQUEST: {
      ApiConnectRequest connectReq;
      io.mapRequired("params", connectReq.params);
      if (!errorFound(io)) conn_->onConnectRequest(&connectReq);
      break;
    }
    case LIBOFP_SET_TIMER: {
      ApiSetTimer setTimer;
      io.mapRequired("params", setTimer.params);
      if (!errorFound(io)) conn_->onSetTimer(&setTimer);
      break;
    }
    case LIBOFP_EDIT_SETTING: {
      ApiEditSetting editSetting;
      io.mapRequired("params", editSetting.params);
      if (!errorFound(io)) conn_->onEditSetting(&editSetting);
      break;
    }
    default:
      log::info("ApiEncoder: Unrecognized event", event);
      break;
  }
}
#endif  // 0
