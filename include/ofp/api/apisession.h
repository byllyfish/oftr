//  ===== ---- ofp/api/apisession.h ------------------------*- C++ -*- =====  //
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
/// \brief Defines the api::ApiSession class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_API_APISESSION_H_
#define OFP_API_APISESSION_H_

#include "ofp/api/apiserver.h"

namespace ofp {
namespace api {

class ApiConnectionSession;

/// Communicate with an ApiServer running in another thread. Subclass to
/// implement receive() to receive data from the thread (receive() is called
/// from the server thread.)

class ApiSession {
 public:
  ApiSession() : server_{&driver_, this} {}
  virtual ~ApiSession() {}

  void run() { driver_.run(); }
  void stop() { driver_.stop(); }

  void send(const std::string &msg);
  virtual void receive(const std::string &msg) = 0;

  void setConnection(const std::shared_ptr<ApiConnectionSession> &conn);

 private:
  Driver driver_;
  // N.B. conn_ is used in initialization of server_. conn_ also depends on
  // driver_, so it must be initialized after driver_.
  std::shared_ptr<ApiConnectionSession> conn_;
  ApiServer server_;
};

}  // namespace api
}  // namespace ofp

#endif  // OFP_API_APISESSION_H_
