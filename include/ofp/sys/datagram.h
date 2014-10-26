//  ===== ---- ofp/datagram.h ------------------------------*- C++ -*- =====  //
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
/// \brief Implements a datagram object suitable for use with asio 
/// async_send_to.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_SYS_DATAGRAM_H_
#define OFP_SYS_DATAGRAM_H_

namespace ofp {
namespace sys {

OFP_BEGIN_IGNORE_PADDING

class Datagram {
public:
    const UInt8 *data() const { return buf_.data(); }
    size_t size() const { return buf_.size(); }

    udp::endpoint destination() const { return dest_;}
    void setDestination(const udp::endpoint &dest) { dest_ = dest; }

    UInt64 connectionId() const { return connId_; }
    void setConnectionId(UInt64 connId) { connId_ = connId; }

    void write(const void *data, size_t length) { buf_.add(data, length); }

private:
    ByteList buf_;
    udp::endpoint dest_;
    UInt64 connId_ = 0;
};

OFP_END_IGNORE_PADDING

}  // namespace sys
}  // namespace ofp

#endif // OFP_SYS_DATAGRAM_H_
