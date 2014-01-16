//  ===== ---- ofp/ofp.cpp ---------------------------------*- C++ -*- =====  //
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
/// \brief Implements high-level runController and runAgent functions.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp.h"

using namespace ofp;

std::error_code ofp::runController(ChannelListener::Factory listenerFactory,
                             ProtocolVersions versions) {
  Driver driver;

  auto ex =
      driver.listen(Driver::Controller, IPv6Endpoint{OFP_DEFAULT_PORT},
                    versions, listenerFactory);

  std::error_code result;
  ex.done([&result](const std::error_code &err) { result = err; });

  driver.run();

  return result;
}

std::error_code ofp::runAgent(const IPv6Address &remoteAddress,
                        ChannelListener::Factory listenerFactory,
                        ProtocolVersions versions) {
  Driver driver;

  auto ex = driver.connect(Driver::Agent,
                           IPv6Endpoint{remoteAddress, OFP_DEFAULT_PORT},
                           versions, listenerFactory);

  std::error_code result;
  ex.done([&result](const std::error_code &err) { result = err; });

  driver.run();

  return result;
}
