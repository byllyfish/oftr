#ifndef OFP_PROTOCOLELEMENT_H_
#define OFP_PROTOCOLELEMENT_H_

namespace ofp {

/// \brief A marker interface for protocol elements.
///
/// A ProtocolElement is a structure containing a 16-bit length, and optionally
/// a 16-bit type. You can use a ProtocolIterator to iterate over an array
/// of ProtocolElements. A range of ProtocolIterators is encapsulated in a
/// ProtocolIterable object.

class ProtocolElement {
public:
    ProtocolElement() = default;
    ProtocolElement(const ProtocolElement &) = delete;
    ProtocolElement &operator=(const ProtocolElement &) = delete;
};

}  // namespace ofp

#endif  // OFP_PROTOCOLELEMENT_H_
