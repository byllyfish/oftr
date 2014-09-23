//  ===== ---- ofp/channeltransport.h ----------------------*- C++ -*- =====  //
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
/// \brief Defines ChannelTransport enum class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_CHANNELTRANSPORT_H_
#define OFP_CHANNELTRANSPORT_H_

namespace ofp {

enum class ChannelTransport {
    None = 0,
    TCP_Plaintext,
    UDP_Plaintext,
    TCP_TLS,
    UDP_DTLS
};

// Specialize this function to return the channel transport for a specific
// socket type.
template <class SocketType>
constexpr ChannelTransport ToChannelTransport() {
    return ChannelTransport::None;
}

}  // namespace ofp

#endif // OFP_CHANNELTRANSPORT_H_
