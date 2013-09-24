//  ===== ---- ofp/hello.cpp -------------------------------*- C++ -*- =====  //
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
/// \brief Implements Hello and HelloBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/hello.h"
#include "ofp/log.h"
#include "ofp/constants.h"
#include "ofp/channel.h"

using namespace ofp;


UInt16 detail::HelloElement::type() const
{
	return type_;
}


void detail::HelloElement::setType(UInt16 type) {
	type_ = type;
}

void detail::HelloElement::setLength(UInt16 length) {
	length_ = length;
}



ProtocolVersions detail::HelloElement::versionBitMap() const
{
	if (length_ - sizeof(HelloElement) >= sizeof(UInt32)) {
        // Grab the first bit map.
	    const Big32 *bitmap = reinterpret_cast<const Big32 *>(BytePtr(this) + sizeof(HelloElement));
        return ProtocolVersions::fromBitmap(*bitmap);
    }
	
    log::info("versionBitMap not big enough");

	return ProtocolVersions::fromBitmap(0);
}

bool detail::HelloElement::validateLength(size_t remaining) const
{
    if (length_ > remaining || (length_ % 8) != 0) {
        log::info("HelloElement validateLength failed", length_);
        return false;
    }
    return true;
}

const detail::HelloElement *detail::HelloElement::next(size_t *remaining) const
{
	assert(*remaining >= length_);
	*remaining -= length_;
	return reinterpret_cast<const HelloElement *>(BytePtr(this) + length_);
}

const Hello *Hello::cast(const Message *message)
{
    assert(message->type() == OFPT_HELLO);

    const Hello *msg = reinterpret_cast<const Hello *>(message->data());
    if (!msg->validateLength(message->size())) {
        return nullptr;
    }

    return msg;
}

bool Hello::validateLength(size_t length) const
{
	using namespace detail;

    if (length < sizeof(Header)) {
        log::info("Hello validateLength failed");
        return false;
    }

    if (length != header_.length()) {
        log::info("Hello validateLength failed");
        return false;
    }

    if (length > sizeof(Header)) {
        const HelloElement *elem = helloElements();
        size_t remaining = length - sizeof(Header);
        while (remaining > 0) {
            if (!elem->validateLength(remaining)) {
                return false;
            }
            elem = elem->next(&remaining);
        }
    }

    return true;
}

ProtocolVersions Hello::protocolVersions() const
{
	using namespace detail;

    UInt16 hdrLen = header_.length();

    if (hdrLen > sizeof(Header)) {
        // Search for version bit map.

        const HelloElement *elem = helloElements();
        size_t remaining = hdrLen - sizeof(Header);

        while (remaining > 0) {
            if (!elem->validateLength(remaining)) {
                return ProtocolVersions::fromBitmap(0);
            }
            if (elem->type() == OFPHET_VERSIONBITMAP) {
                return elem->versionBitMap();
            }
            elem = elem->next(&remaining);
        }
    }

    return ProtocolVersions{header_.version()};
}


const detail::HelloElement *Hello::helloElements() const
{
    return reinterpret_cast<const detail::HelloElement *>(BytePtr(&header_) + sizeof(header_));
}

UInt32 HelloBuilder::send(Writable *channel)
{   
    // HelloRequest is the only message type that doesn't use the channel's
    // version number. If the highest version number is 4, don't send any 
    // HelloElements; POX doesn't support the extended part of the Hello
    // request.

	UInt32 xid = channel->nextXid();

    Header &hdr = msg_.header_;
	size_t msgLen = sizeof(msg_);
    if (hdr.version() >= OFP_VERSION_4) {
        // Include our hello element.
	    msgLen += sizeof(elem_) + sizeof(bitmap_);
	}
	
	hdr.setLength(UInt16_narrow_cast(msgLen));
	hdr.setXid(xid);

	elem_.setType(OFPHET_VERSIONBITMAP);
	elem_.setLength(sizeof(elem_) + sizeof(bitmap_));

	channel->write(this, msgLen);
    channel->flush();

	return xid;
}


