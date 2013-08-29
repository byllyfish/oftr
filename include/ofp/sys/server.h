//  ===== ---- ofp/sys/server.h ----------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines an interface for server subclasses TCP_Server and UDP_Server.
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
