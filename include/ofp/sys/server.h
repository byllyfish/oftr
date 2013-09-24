//  ===== ---- ofp/sys/server.h ----------------------------*- C++ -*- =====  //
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
/// \brief Defines the abstract sys::Server class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_SYS_SERVER_H
#define OFP_SYS_SERVER_H

namespace ofp { // <namespace ofp>
namespace sys { // <namespace sys>

/// \brief Interface for server classes.
class Server {
public:
	virtual ~Server() {}
};

} // </namespace sys>
} // </namespace ofp>

#endif // OFP_SYS_SERVER_H
