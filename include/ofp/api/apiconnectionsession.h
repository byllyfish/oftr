//  ===== ---- ofp/api/apiconnectionsession.h --------------*- C++ -*- =====  //
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
/// \brief Defines the api::ApiConnectionSession class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_API_APICONNECTIONSESSION_H_
#define OFP_API_APICONNECTIONSESSION_H_

#include "ofp/sys/asio_utils.h"
#include "ofp/api/apiconnection.h"

namespace ofp {
namespace api {

OFP_BEGIN_IGNORE_PADDING

class ApiConnectionSession : public ApiConnection {
 public:
  ApiConnectionSession(ApiServer *server, ApiSession *session);

  void asyncAccept() override {}

 protected:
  void write(const std::string &msg) override;
  void asyncRead() override;

 private:
  ApiSession *session_;
  asio::io_service::work work_;
};

OFP_END_IGNORE_PADDING

}  // namespace api
}  // namespace ofp

#endif  // OFP_API_APICONNECTIONSESSION_H_
