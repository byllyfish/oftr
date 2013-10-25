//  ===== ---- ofp/connectiontimer.cpp ---------------------*- C++ -*- =====  //
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
/// \brief Implements ConnectionTimer class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/sys/connectiontimer.h"
#include "ofp/sys/connection.h"
#include "ofp/sys/engine.h"

namespace ofp { // <namespace ofp>
namespace sys { // <namespace sys>

ConnectionTimer::ConnectionTimer(Connection *conn, UInt32 id,
                                 milliseconds interval, bool repeating)
    : conn_{conn}, timer_{conn->engine()->io()}, interval_{interval}, id_{id},
      repeating_{repeating} {
  asyncWait();
}

void ConnectionTimer::asyncWait() {
  log::debug("ConnectionTimer::asyncWait");

  timer_.expires_from_now(interval_);
  timer_.async_wait([this](const error_code & err) {
    if (err != boost::asio::error::operation_aborted) {
      // If we're repeating, restart the timer before posting it. It's
      // possible the client will want to cancel the timer when they
      // receive its event.
      if (repeating_) {
        asyncWait();
      }
      conn_->postTimerExpired(this);
    }
  });
}

} // </namespace sys>
} // </namespace ofp>
