//  ===== ---- ofp/hello.h ---------------------------------*- C++ -*- =====  //
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
/// \brief Defines the Hello and HelloBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_HELLO_H
#define OFP_HELLO_H

#include "ofp/protocolmsg.h"
#include "ofp/protocolversions.h"

namespace ofp { // <namespace ofp>
namespace detail { // <namespace detail>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //
//   d e t a i l . H e l l o E l e m e n t
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //

class HelloElement {
public:
    UInt16 type() const;

    void setType(UInt16 type);
    void setLength(UInt16 length);

    ProtocolVersions versionBitMap() const;
    bool validateLength(size_t remaining) const;
    const HelloElement *next(size_t *remaining) const;

private:
    Big16 type_;
    Big16 length_;
};

} // </namespace detail>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //
//   H e l l o 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //
/// \brief Immutable OpenFlow `Hello` protocol message.

class Hello : public ProtocolMsg<Hello,OFPT_HELLO> {
public:
    ProtocolVersions protocolVersions() const;

    bool validateLength(size_t length) const;

private:
    Header header_;

    const detail::HelloElement *helloElements() const;

    // Only HelloBuilder can construct an actual instance.
    Hello() : header_{type()}
    {
    }

    friend class HelloBuilder;
};

static_assert(sizeof(Hello) == 8, "Unexpected size.");
static_assert(IsStandardLayout<Hello>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<Hello>(), "Expected trivially copyable.");

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //
//   H e l l o B u i l d e r
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //
/// \brief Mutable builder for an OpenFlow `Hello` protocol message.

class HelloBuilder {
public:

    HelloBuilder(ProtocolVersions versions = ProtocolVersions{})
        : bitmap_{versions.bitmap()}
    {
        msg_.header_.setVersion(versions.highestVersion());
    }

    explicit HelloBuilder(UInt8 version)
    	: bitmap_{ProtocolVersions{version}.bitmap()}
    {
        if (version == 0)
            version = OFP_VERSION_LAST;
    	msg_.header_.setVersion(version);
    }

    explicit HelloBuilder(const Hello *msg);

    void setProtocolVersions(ProtocolVersions versions)
    {
        bitmap_ = versions.bitmap();
        msg_.header_.setVersion(versions.highestVersion());
    }

    UInt32 send(Writable *channel);

private:
    Hello msg_;
    detail::HelloElement elem_;
    Big32 bitmap_ = 0;
};

} // </namespace ofp>

#endif // OFP_HELLO_H
