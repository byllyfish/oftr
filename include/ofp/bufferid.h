#ifndef OFP_BUFFERID_H_
#define OFP_BUFFERID_H_

namespace ofp {

enum OFPBufferID : UInt32;

class BufferID {
public:
    constexpr BufferID() = default;
    constexpr BufferID(UInt32 id) : id_{id} {}

    // This is a convenience constructor (for efficiency).
    constexpr BufferID(Big32 id) : id_{id} {}

    constexpr operator OFPBufferID() const { return static_cast<OFPBufferID>(value()); }

    bool operator==(const BufferID &rhs) const { return id_ == rhs.id_; }
    bool operator!=(const BufferID &rhs) const { return !(*this == rhs); }

private:
    Big32 id_;

    constexpr UInt32 value() const { return id_; }
};

}  // namespace ofp

#endif // OFP_BUFFERID_H_
