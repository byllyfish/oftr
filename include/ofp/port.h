//  ===== ---- ofp/port.h ----------------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the Port class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_PORT_H
#define OFP_PORT_H

#include "ofp/byteorder.h"
#include "ofp/smallcstring.h"
#include "ofp/constants.h"
#include "ofp/padding.h"
#include "ofp/enetaddress.h"

namespace ofp { // <namespace ofp>

namespace deprecated { // <namespace deprecated>
class PortV1;
} // </namespace deprecated>

using PortName = SmallCString<OFP_MAX_PORT_NAME_LEN>;

class Port {
public:
    Port() = default;
    Port(const deprecated::PortV1 &port);

    UInt32 portNo() const
    {
        return portNo_;
    }
    void setPortNo(UInt32 portNo)
    {
        portNo_ = portNo;
    }

    const EnetAddress &hwAddr() const
    {
        return hwAddr_;
    }
    void setHwAddr(const EnetAddress &hwAddr)
    {
        hwAddr_ = hwAddr;
    }

    const PortName &name() const
    {
        return name_;
    }
    void setName(const PortName &name)
    {
        name_ = name;
    }

    UInt32 config() const
    {
        return config_;
    }
    void setConfig(UInt32 config)
    {
        config_ = config;
    }

    UInt32 state() const
    {
        return state_;
    }
    void setState(UInt32 state)
    {
        state_ = state;
    }

    UInt32 curr() const
    {
        return curr_;
    }
    void setCurr(UInt32 curr)
    {
        curr_ = curr;
    }

    UInt32 advertised() const
    {
        return advertised_;
    }
    void setAdvertised(UInt32 advertised)
    {
        advertised_ = advertised;
    }

    UInt32 supported() const
    {
        return supported_;
    }
    void setSupported(UInt32 supported)
    {
        supported_ = supported;
    }

    UInt32 peer() const
    {
        return peer_;
    }
    void setPeer(UInt32 peer)
    {
        peer_ = peer;
    }

    UInt32 currSpeed() const
    {
        return currSpeed_;
    }
    void setCurrSpeed(UInt32 currSpeed)
    {
        currSpeed_ = currSpeed;
    }

    UInt32 maxSpeed() const
    {
        return maxSpeed_;
    }
    void setMaxSpeed(UInt32 maxSpeed)
    {
        maxSpeed_ = maxSpeed;
    }

private:
    Big32 portNo_;
    Padding<4> pad1_;
    EnetAddress hwAddr_;
    Padding<2> pad2_;
    PortName name_;
    Big32 config_;
    Big32 state_;
    Big32 curr_;
    Big32 advertised_;
    Big32 supported_;
    Big32 peer_;
    Big32 currSpeed_;
    Big32 maxSpeed_;
};

static_assert(sizeof(Port) == 64, "Unexpected size.");
static_assert(IsStandardLayout<Port>(), "Expected standard layout.");

namespace deprecated { // <namespace deprecated>

class PortV1 {
public:
    PortV1() = default;
    explicit PortV1(const Port &port);

    UInt16 portNo() const
    {
        return portNo_;
    }
    void setPortNo(UInt16 portNo)
    {
        portNo_ = portNo;
    }

    const EnetAddress &hwAddr() const
    {
        return hwAddr_;
    }
    void setHwAddr(const EnetAddress &hwAddr)
    {
        hwAddr_ = hwAddr;
    }

    const PortName &name() const
    {
        return name_;
    }
    void setName(const PortName &name)
    {
        name_ = name;
    }

    UInt32 config() const
    {
        return config_;
    }
    void setConfig(UInt32 config)
    {
        config_ = config;
    }

    UInt32 state() const
    {
        return state_;
    }
    void setState(UInt32 state)
    {
        state_ = state;
    }

    UInt32 curr() const
    {
        return curr_;
    }
    void setCurr(UInt32 curr)
    {
        curr_ = curr;
    }

    UInt32 advertised() const
    {
        return advertised_;
    }
    void setAdvertised(UInt32 advertised)
    {
        advertised_ = advertised;
    }

    UInt32 supported() const
    {
        return supported_;
    }
    void setSupported(UInt32 supported)
    {
        supported_ = supported;
    }

    UInt32 peer() const
    {
        return peer_;
    }
    void setPeer(UInt32 peer)
    {
        peer_ = peer;
    }

private:
    Big16 portNo_;
    EnetAddress hwAddr_;
    PortName name_;
    Big32 config_;
    Big32 state_;
    Big32 curr_;
    Big32 advertised_;
    Big32 supported_;
    Big32 peer_;
};

static_assert(sizeof(PortV1) == 48, "Unexpected size.");
static_assert(IsStandardLayout<PortV1>(), "Expected standard layout.");

} // </namespace deprecated>
} // </namespace ofp>

#endif // OFP_PORT_H
