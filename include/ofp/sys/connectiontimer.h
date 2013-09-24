//  ===== ---- ofp/sys/connectiontimer.h -------------------*- C++ -*- =====  //
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
/// \brief Defines the sys::ConnectionTimer class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_SYS_CONNECTION_TIMER_H
#define OFP_SYS_CONNECTION_TIMER_H

#include "ofp/sys/boost_asio.h"

namespace ofp { // <namespace ofp>
namespace sys { // <namespace sys>

class Connection;

OFP_BEGIN_IGNORE_PADDING

/// \brief Implements a timer acting on a specified connection. Each timer has a
/// unique ID.
class ConnectionTimer {
public:
	ConnectionTimer(Connection *conn, UInt32 id, milliseconds interval, bool repeating);

	UInt32 id() const { return id_; }
	bool repeating() const { return repeating_; }

private:
	Connection *conn_;
	steady_timer timer_;
	milliseconds interval_;
    UInt32 id_;
    bool repeating_;

    void asyncWait();
};

OFP_END_IGNORE_PADDING

/// \brief Stores connection timers so we can look them up by ID.
using ConnectionTimerMap = std::map<UInt32,std::unique_ptr<ConnectionTimer>>;

} // </namespace sys>
} // </namespace ofp>

#endif // OFP_SYS_CONNECTION_TIMER_H
