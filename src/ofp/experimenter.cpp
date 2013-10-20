//  ===== ---- ofp/experimenter.cpp ------------------------*- C++ -*- =====  //
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
/// \brief Implements Experimenter and ExperimenterBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/experimenter.h"
#include "ofp/message.h"
#include "ofp/writable.h"
#include "ofp/padding.h"

namespace ofp { // <namespace ofp>

ByteRange Experimenter::expData() const
{
    return ByteRange{BytePtr(this) + sizeof(Experimenter),
                     header_.length() - sizeof(Experimenter)};
}


bool Experimenter::validateLength(size_t length) const
{
	return length >= sizeof(Experimenter);
}

ExperimenterBuilder::ExperimenterBuilder(const Experimenter *msg)
    : msg_{*msg}
{
    ByteRange expData = msg->expData();
    setExpData(expData.data(), expData.size());
}

UInt32 ExperimenterBuilder::send(Writable *channel)
{
    UInt8 version = channel->version();
    UInt32 xid = channel->nextXid();
    size_t msgLen = sizeof(msg_) + data_.size();

    if (version == OFP_VERSION_1)
        msgLen -= 4;

    msg_.header_.setVersion(version);
    msg_.header_.setXid(xid);
    msg_.header_.setLength(UInt16_narrow_cast(msgLen));

    if (version == OFP_VERSION_1) {
        // Header + experimenter (12 bytes)
        channel->write(&msg_, 12);
    } else if (version < OFP_VERSION_4) {
        // Header + experimenter + 4 byte pad (12 + 4 bytes)
        Padding<4> pad;
        channel->write(&msg_, 12);
        channel->write(&pad, 4);

    } else {
        // Header + experimenter + expType (16 bytes)
        channel->write(&msg_, sizeof(msg_));
    }

    channel->write(data_.data(), data_.size());
    channel->flush();

    return xid;
}

} // </namespace ofp>