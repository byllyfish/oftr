//  ===== ---- ofp/message.cpp -----------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Implements Message class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/message.h"
#include "ofp/sys/connection.h"
#include "ofp/flowmod.h"
#include "ofp/portstatus.h"
#include "ofp/experimenter.h"
#include "ofp/instructions.h"
#include "ofp/instructionrange.h"
#include "ofp/originalmatch.h"
#include "ofp/transmogrify.h"

namespace ofp { // <namespace ofp>

Channel *Message::source() const
{
    return channel_;
}

void Message::transmogrify()
{
    Transmogrify tr{this};
    tr.normalize();
}

#if 0
/// \brief Modify the binary representation of the message to fit our standard
/// layout protocol interface. This method changes a version 1 FlowMod
/// representation into the version 2+ FlowMod representation. It leaves other
/// messages untouched. (This strategy assumes that the v1 and v2 protocol specs
/// will not change retroactively.)
///

void Message::transmogrify()
{
    assert(size() >= sizeof(Header));
    
    // Caution! Many magic numbers ahead...

    Header *hdr = header();

    // Translate type of message from earlier version into latest enum.
    OFPType type = Header::translateType(hdr->version(), hdr->type(), OFP_VERSION_4);
    if (type == OFPT_UNSUPPORTED) {
        log::info("Unsupported type for protocol version:", hdr->type());
    }
    hdr->setType(type);
    
    if (hdr->version() == OFP_VERSION_1) {
        if (hdr->type() == FlowMod::type()) {
            transmogrifyFlowModV1();
        } else if (hdr->type() == PortStatus::type()) {
            transmogrifyPortStatusV1();
        } else if (hdr->type() == Experimenter::type()) {
            transmogrifyExperimenterV1();
        }
    }
}

void Message::transmogrifyFlowModV1()
{
    using deprecated::OriginalMatch;
    using deprecated::StandardMatch;

    Header *hdr = header();

    if (hdr->length() < 72) {
        log::info("FlowMod v1 message is too short.", hdr->length());
        hdr->setType(OFPT_UNSUPPORTED);
        return;
    }

    // To convert a v1 FlowMod into v2:
    //  1. Copy OriginalMatch into StandardMatch.
    //  1. Add 64-bit cookie_mask field after cookie.
    //  2. Split 16-bit command field into 8-bit table_id and 8-bit command.
    //  3. Change out_port from 16 to 32 bits (sign-extend if necessary).
    //  4. Add 32 bit out_group field after out_port.
    //  5. Add 2 pad bytes after flag.
    //  6. Insert StandardMatch after new pad bytes.

    UInt8 *pkt = buf_.mutableData();
    size_t origSize = buf_.size();

    OriginalMatch *origMatch =
        reinterpret_cast<OriginalMatch *>(pkt + sizeof(Header));

    StandardMatch stdMatch{*origMatch};

    // Specify cookieMask as all 1's.
    UInt64 cookie_mask = ~0ULL;

    Big16 *out_port16 = reinterpret_cast<Big16 *>(pkt + 68);
    UInt32 out_port32 = *out_port16;
    if (out_port32 > 0xFF00U) {
        // Sign extend to 32-bits the "fake" ports.
        out_port32 |= 0xFFFF0000U;
    }
    Big32 out_port = out_port32;

    std::memcpy(pkt + 8, pkt + 48, 8);      // set cookie
    std::memcpy(pkt + 16, &cookie_mask, 8); // set cookie_mask
    std::memcpy(pkt + 24, pkt + 56, 12);    // set up to out_port
    std::memcpy(pkt + 36, &out_port, 4);    // set out_port as 32 bits
    std::memset(pkt + 40, 0, 4);            // set out_group
    std::memcpy(pkt + 44, pkt + 70, 2);     // set flags
    std::memset(pkt + 46, 0, 2);            // set 2 pad bytes

    // Current position is pkt + 48.
    // StandardMatch size is 88.
    // Actions start at pkt + 72.
    // We need to convert this actionlist into an ApplyActions instruction.
    //   1. Insert type, length and 4 bytes (8 bytes total)
    // We need to insert 64 bytes + 8 bytes.

    buf_.insertUninitialized(pkt + 48, 72);

    // This insertion may move memory; update pkt ptr just in case.
    // Copy in StandardMatch.

    pkt = buf_.mutableData();
    std::memcpy(pkt + 48, &stdMatch, sizeof(stdMatch));

    assert(origSize >= 72);
    UInt16 actLen = UInt16_narrow_cast(origSize - 72);
    detail::InstructionHeaderWithPadding insHead{
        OFPIT_APPLY_ACTIONS, actLen};

    std::memcpy(pkt + 120, &insHead, sizeof(insHead));

    // Update header length. N.B. Make sure we use current header ptr.
    header()->setLength(UInt16_narrow_cast(size()));
}

void Message::transmogrifyPortStatusV1()
{
    using deprecated::PortV1;

    Header *hdr = header();

    if (hdr->length() != 64) {
        log::info("PortStatus v1 message is wrong size.", hdr->length());
        hdr->setType(OFPT_UNSUPPORTED);
        return;
    }

    UInt8 *pkt = buf_.mutableData();

    PortV1 *portV1 = reinterpret_cast<PortV1 *>(pkt + 16);
    Port port{*portV1};

    buf_.addUninitialized(sizeof(Port) - sizeof(PortV1));

    // The insertion may move memory; update `pkt` just in case.
    pkt = buf_.mutableData();
    std::memcpy(pkt + 16, &port, sizeof(Port));

    assert(buf_.size() == sizeof(PortStatus));
}

void Message::transmogrifyExperimenterV1()
{
    Header *hdr = header();

    if (hdr->length() < 12) {
        log::info("Experimenter v1 message is too short.", hdr->length());
        hdr->setType(OFPT_UNSUPPORTED);
        return;
    }

    // Insert four zero bytes at position 12.
    Padding<4> pad;
    buf_.insert(buf_.data() + 12, &pad, sizeof(pad));

    // Update header length. N.B. Make sure we use current header ptr.
    header()->setLength(UInt16_narrow_cast(size()));
}


#if 0
int Message::transmogrifyInstructionsV1orV2(const InstructionRange &instr)
{
    // Return change in length of instructions list.
    

}

// This method may move memory.
int Message::transmogrifyActionsV1orV2(const ActionRange &actions)
{
    using namespace deprecated;

    // Return change in length of action list.
    int lengthChange = 0;

    ActionIterator iter = actions.begin();
    ActionIterator iterEnd = actions.end();

    while (iter < iterEnd) {
        ActionType actType = iter.type();
        OFPActionType type = actType.type();

        if (type >= ATv1::OFPAT_SET_VLAN_VID && type <= ATv1::OFPAT_ENQUEUE ||
             type == ATv2::OFPAT_SET_MPLS_LABEL || type == ATv2::OFPAT_SET_MPLS_TC) {
            lengthChange += transmogrifyActionV1orV2(type, &iter, &iterEnd);
        }
        ++iter;
    }

    return lengthChange;
}


int Message::transmogrifyActionV1orV2(UInt16 type, ActionIterator *iter, ActionIterator *iterEnd)
{
    using namespace deprecated;

    UInt8 version = version();
    if (version == OFP_VERSION_1) {
        // Normalize action type code to the V2+ schema.
        if (type == ATv1::OFPAT_STRIP_VLAN) {
            type = OFPAT_STRIP_VLAN_V1;
        } else if (type == ATv1::OFPAT_ENQUEUE) {
            type = OFPAT_ENQUEUE_V1;
        } else if (type >= ATv1::OFPAT_SET_DL_SRC && type <= OFPAT_SET_NW_TOS) {
            --type;
        }
    }

    int lengthChange = 0;
    switch (type)
    {
        case ATv2::OFPAT_SET_VLAN_VID:
            lengthChange += transmogrifyToSetField<OFB_VLAN_VID>(iter);
            break;
    }

    return lengthChange;
}

template <class Type>
int Message::transmogrifyToSetField(UInt16 type, ActionIterator *iter, ActionIterator *iterEnd)
{
    size_t valueLen = iter->valueSize();

    if (valueLen == sizeof(Type)) {
        Type::NativeValue value;
        memcpy(&value, iter->valuePtr(), valueLen);
        
        OXMList list;
        list.add(Type{value});
        list.pad();

        int lengthChange = list.size() - valueLen
        if (lengthChange > 0) {
            ByteListPreservePtr m1{buf_, iter};
            buf_.insertUninitialized(iter->valuePtr(), lengthChange);
            *iterEnd = buf_.end();
        } else if (lengthChange < 0) {
            ByteListPreservePtr m1{buf_, iter};
            buf_.remove(-lengthChange);
            *iterEnd = buf_.end();
        }

        memcpy(iter->valuePtr(), list.data(), list.size());
        ActionType setField{OFPAT_SET_FIELD, list.size()};
        memcpy(iter->data(), &setField, sizeof(ActionType));

    } else {
        log::info("transmogrifyToSetField: Unexpected value size.")
    }
}
#endif //0
#endif //0

} // </namespace ofp>