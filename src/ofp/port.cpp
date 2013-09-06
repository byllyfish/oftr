#include "ofp/port.h"

namespace ofp { // <namespace ofp>


Port::Port(const deprecated::PortV1 &port)
{
	portNo_ = port.portNo();
    hwAddr_ = port.hwAddr();
    name_ = port.name();
    config_ = port.config();
    state_ = port.state();
    curr_ = port.curr();
    advertised_ = port.advertised();
    supported_ = port.supported();
    peer_ = port.peer();
    currSpeed_ = 0;
    maxSpeed_ = 0;	
}


namespace deprecated { // <namespace deprecated>


PortV1::PortV1(const Port &port)
{
	portNo_ = UInt16_narrow_cast(port.portNo());
    hwAddr_ = port.hwAddr();
    name_ = port.name();
    config_ = port.config();
    state_ = port.state();
    curr_ = port.curr();
    advertised_ = port.advertised();
    supported_ = port.supported();
    peer_ = port.peer();
}


} // </namespace deprecated>
} // </namespace ofp>