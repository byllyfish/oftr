//  ===== ---- ofp/protocolmsg.h ---------------------------*- C++ -*- =====  //
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
/// \brief Defines the ProtocolMsg template class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_PROTOCOLMSG_H
#define OFP_PROTOCOLMSG_H

#include "ofp/header.h"
#include "ofp/message.h"

namespace ofp { // <namespace ofp>

/// \brief Static base class template for all protocol message types. This class
/// implements the two static methods, type() and cast(), common to all protocol
/// message types.

template <class MsgClass, OFPType MsgType>
class ProtocolMsg {
public:
  /// \returns OpenFlow message type reprented by this class.
  static constexpr OFPType type() { return MsgType; }

  /// \returns Pointer to message memory as given class or nullptr.
  static const MsgClass *cast(const Message *message) {
    return message->cast<MsgClass>();
  }
};

} // </namespace ofp>

#endif // OFP_PROTOCOLMSG_H
