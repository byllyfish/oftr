//  ===== ---- ofp/hello.h ---------------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the Hello and HelloBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_HELLO_H
#define OFP_HELLO_H

#include "ofp/header.h"
#include "ofp/protocolversions.h"
#include "ofp/message.h"

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
class Hello {
public:
    static constexpr OFPType type()
    {
        return OFPT_HELLO;
    }

    Hello() : header_{type()}
    {
    }

    static const Hello *cast(const Message *message);

    ProtocolVersions protocolVersions() const;

private:
    Header header_;

    bool validateLength(size_t length) const;
    const detail::HelloElement *helloElements() const;

    friend class HelloBuilder;
};

static_assert(sizeof(Hello) == 8, "Unexpected size.");
static_assert(IsStandardLayout<Hello>(), "Expected standard layout.");

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
    	: bitmap_{ProtocolVersions{}.bitmap()}
    {
    	msg_.header_.setVersion(version);
    }

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
