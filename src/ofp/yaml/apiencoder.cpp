//  ===== ---- ofp/yaml/apiencoder.cpp ---------------------*- C++ -*- =====  //
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
/// \brief Implements yaml::ApiEncoder class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/yaml/apiencoder.h"
#include "ofp/yaml/apiconnection.h"

using namespace ofp::yaml;

ApiEncoder::ApiEncoder(const std::string &input, ApiConnection *conn)
    : conn_{conn}, errorStream_{error_} {
  if (!input.empty()) {
    llvm::yaml::Input yin{input, nullptr, ApiEncoder::diagnosticHandler, this};
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
    io.mapRequired("msg", loopback.msg);
    conn_->onLoopback(&loopback);
    break;
  }
  case LIBOFP_LISTEN_REQUEST: {
    ApiListenRequest listenReq;
    io.mapRequired("msg", listenReq.msg);
    if (listenReq.msg.listenPort != 0)
      conn_->onListenRequest(&listenReq);
    break;
  }
  case LIBOFP_SET_TIMER: {
    ApiSetTimer setTimer;
    io.mapRequired("msg", setTimer.msg);
    conn_->onSetTimer(&setTimer);
    break;
  }
  case LIBOFP_EDIT_SETTING: {
    ApiEditSetting editSetting;
    io.mapRequired("msg", editSetting.msg);
    conn_->onEditSetting(&editSetting);
    break;
  }
  default:
    log::info("ApiEncoder: Unrecognized event", event);
    break;
  }
}
