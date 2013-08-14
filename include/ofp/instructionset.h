#ifndef OFP_INSTRUCTIONSET_H
#define OFP_INSTRUCTIONSET_H

#include "ofp/types.h"
#include "ofp/instructions.h"
#include <vector>

namespace ofp { // <namespace ofp>

class InstructionSet {
public:

    InstructionSet() = default;

    const UInt8 *data() const
    {
        return &buf_[0];
    }
    
    size_t size() const
    {
        return buf_.size();
    }

    template <class Type>
    void add(const Type &instruction);

private:
    std::vector<UInt8> buf_;

    void add(const void *data, size_t length)
    {
        const UInt8 *ptr = static_cast<const UInt8 *>(data);
        buf_.insert(buf_.end(), ptr, ptr + length);
    }
};

template <class Type>
inline void InstructionSet::add(const Type &instruction)
{
    add(&instruction, sizeof(instruction));
}

template <>
inline void InstructionSet::add(const IT_WRITE_ACTIONS &instruction)
{
    add(&instruction, IT_WRITE_ACTIONS::HeaderSize);
    add(instruction.data(), instruction.size());
}

template <>
inline void InstructionSet::add(const IT_APPLY_ACTIONS &instruction)
{
    add(&instruction, IT_APPLY_ACTIONS::HeaderSize);
    add(instruction.data(), instruction.size());
}

} // </namespace ofp>

#endif // OFP_INSTRUCTIONSET_H
