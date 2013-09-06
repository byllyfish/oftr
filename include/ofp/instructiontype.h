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

namespace ofp { // <namespace ofp>

class InstructionType {
public:

    enum : UInt16{ IT_GOTO_TABLE = 1,       IT_WRITE_METADATA = 2,
                   IT_WRITE_ACTIONS = 3,    IT_APPLY_ACTIONS = 4,
                   IT_CLEAR_ACTIONS = 5,    IT_METER = 6,
                   IT_EXPERIMENTER = 0xFFFF };

    constexpr InstructionType(UInt16 type) : type_{ type }
    {
    }

    static InstructionType fromBytes(const UInt8 *data);

    constexpr UInt16 type() const
    {
        return type_;
    }

    bool operator==(const InstructionType &rhs) const
    {
        return type_ == rhs.type_;
    }

    bool operator!=(const InstructionType &rhs) const
    {
        return !operator==(rhs);
    }

private:
    const Big16 type_;
};

static_assert(IsLiteralType<InstructionType>(), "Literal type expected.");

std::ostream &operator<<(std::ostream &os, const InstructionType &value);

inline InstructionType InstructionType::fromBytes(const UInt8 *data)
{
    InstructionType type{ 0 };
    std::memcpy(&type, data, sizeof(type));
    return type;
}

inline std::ostream &operator<<(std::ostream &os, const InstructionType &value)
{
    return os << value.type();
}

} // </namespace ofp>

#endif // OFP_INSTRUCTIONTYPE_H
