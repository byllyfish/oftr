#ifndef OFP_PORTNUMBER_H_
#define OFP_PORTNUMBER_H_

#include "ofp/byteorder.h"

namespace ofp {

enum OFPPortNo : UInt32;

class PortNumber {
public:
    constexpr PortNumber() = default;
    constexpr PortNumber(UInt32 port) : port_{port} {}

    // This is a convenience constructor (for efficiency).
    constexpr PortNumber(Big32 port) : port_{port} {}

    constexpr operator OFPPortNo() const { return static_cast<OFPPortNo>(value()); }

    bool operator==(const PortNumber &rhs) const { return port_ == rhs.port_; }
    bool operator!=(const PortNumber &rhs) const { return !(*this == rhs); }

private:
    Big32 port_;

    UInt32 value() const { return port_; }
};

static_assert(sizeof(PortNumber) == 4, "Unexpected size.");

}  // namespace ofp

#endif // OFP_PORTNUMBER_H_
