//  ===== ---- ofp/portstatus.h ----------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the PortStatus and PortStatusBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_PORTSTATUS_H
#define OFP_PORTSTATUS_H

#include "ofp/port.h"
#include "ofp/header.h"
#include "ofp/writable.h"

namespace ofp { // <namespace ofp>

class Message;


class PortStatus {
public:
    static constexpr OFPType type() { return OFPT_PORT_STATUS; }

    static const PortStatus *cast(const Message *message);

    PortStatus() : header_{ type() }
    {
    }

    UInt8 reason() const { return reason_; }
    const Port &port() const { return port_; }
    
    bool validateLength(size_t length) const;

private:
    Header header_;
    UInt8 reason_;
    Padding<7> pad_;
    Port port_;

    friend class PortStatusBuilder;
};

static_assert(sizeof(PortStatus) == 80, "Unexpected size.");
static_assert(IsStandardLayout<PortStatus>(), "Expected standard layout.");

class PortStatusBuilder {
public:
    PortStatusBuilder() = default;

    void setReason(UInt8 reason) { msg_.reason_ = reason; }
    void setPort(const Port &port) { msg_.port_ = port; }

    UInt32 send(Writable *channel);

private:
    PortStatus msg_;
};

} // </namespace ofp>

#endif // OFP_PORTSTATUS_H
