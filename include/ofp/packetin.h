//  ===== ---- ofp/packetin.h ------------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the PacketIn and PacketInBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_PACKETIN_H
#define OFP_PACKETIN_H

#include "ofp/header.h"
#include "ofp/message.h"
#include "ofp/match.h"
#include "ofp/matchbuilder.h"
#include "ofp/writable.h"

namespace ofp { // <namespace ofp>

class PacketInBuilder;

class PacketIn {
public:
    static constexpr OFPType type() { return OFPT_PACKET_IN; }

    static const PacketIn *cast(const Message *message)
    {
        return message->cast<PacketIn>();
    }

    PacketIn() : header_{type()}
    {
    }

    UInt8 version() const { return header_.version(); }

    UInt32 bufferId() const
    {
        return bufferId_;
    }

    UInt16 totalLen() const;
    UInt32 inPort() const;
    UInt32 inPhyPort() const;
    UInt64 metadata() const;
    OFPPacketInReason reason() const;
    UInt8 tableID() const;
    UInt64 cookie() const;

    ByteRange enetFrame() const;

    bool validateLength(size_t length) const;

private:
    Header header_;
    Big32 bufferId_;
    Big16 totalLen_;
    OFPPacketInReason reason_;
    Big8 tableID_;
    Big64 cookie_;

    Big16 matchType_ = 0;
    Big16 matchLength_ = 0;
    Padding<4> pad_;

    OXMRange oxmRange() const;

    template <class Type>
    Type offset(size_t offset) const
    {
        return *reinterpret_cast<const Type *>(BytePtr(this) + offset);
    }

    bool validateLengthV1(size_t length) const;
    bool validateLengthV2(size_t length) const;
    bool validateLengthV3(size_t length) const;

    enum : size_t {
        UnpaddedSizeWithMatchHeader = 28,
        SizeWithoutMatchHeader = 24,
        MatchHeaderSize = 4,
    };

    friend class PacketInBuilder;
    friend struct llvm::yaml::MappingTraits<PacketInBuilder>;
};

static_assert(sizeof(PacketIn) == 32, "Unexpected size.");
static_assert(IsStandardLayout<PacketIn>(), "Expected standard layout.");

class PacketInBuilder {
public:
    PacketInBuilder() = default;

    void setBufferId(UInt32 bufferId) { msg_.bufferId_ = bufferId; }
    void setTotalLen(UInt16 totalLen) { msg_.totalLen_ = totalLen; }
    void setInPort(UInt32 inPort) { inPort_ = inPort; }
    void setInPhyPort(UInt32 inPhyPort) { inPhyPort_ = inPhyPort; }
    void setMetadata(UInt64 metadata) { metadata_ = metadata; }
    void setReason(OFPPacketInReason reason) { msg_.reason_ = reason; }
    void setTableID(UInt8 tableID) { msg_.tableID_ = tableID; }
    void setCookie(UInt64 cookie) { msg_.cookie_ = cookie; }

    void setEnetFrame(const ByteList &enetFrame)
    {
        enetFrame_ = enetFrame;
    }

    UInt32 send(Writable *channel);

private:
    PacketIn msg_;
    Big32 inPort_ = 0;
    Big32 inPhyPort_ = 0;
    Big64 metadata_ = 0;
    MatchBuilder match_;
    ByteList enetFrame_;

    UInt32 sendV1(Writable *channel);
    UInt32 sendV2(Writable *channel);
    UInt32 sendV3(Writable *channel);

    friend struct llvm::yaml::MappingTraits<PacketInBuilder>;
};

} // </namespace ofp>

#endif // OFP_PACKETIN_H
