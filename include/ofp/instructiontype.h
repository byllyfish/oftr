//  ===== ---- ofp/instructiontype.h -----------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the InstructionType class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_INSTRUCTIONTYPE_H
#define OFP_INSTRUCTIONTYPE_H

#include "ofp/byteorder.h"
#include "ofp/constants.h"

namespace ofp { // <namespace ofp>

class InstructionType {
public:

    constexpr InstructionType(OFPInstructionType type) : type_{type}
    {
    }

    static InstructionType fromBytes(const UInt8 *data);

    constexpr OFPInstructionType type() const
    {
        return type_;
    }

    constexpr operator OFPInstructionType() const { return type_; }

    bool operator==(const InstructionType &rhs) const
    {
        return type_ == rhs.type_;
    }

    bool operator!=(const InstructionType &rhs) const
    {
        return !operator==(rhs);
    }
    
private:
    const Big<OFPInstructionType> type_;
};

static_assert(IsLiteralType<InstructionType>(), "Literal type expected.");

std::ostream &operator<<(std::ostream &os, const InstructionType &value);

inline InstructionType InstructionType::fromBytes(const UInt8 *data)
{
    InstructionType type{OFPIT_GOTO_TABLE};
    std::memcpy(&type, data, sizeof(type));
    return type;
}

inline std::ostream &operator<<(std::ostream &os, const InstructionType &value)
{
    return os << static_cast<unsigned>(value.type());
}

} // </namespace ofp>

#endif // OFP_INSTRUCTIONTYPE_H
