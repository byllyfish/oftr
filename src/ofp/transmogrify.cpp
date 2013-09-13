#include "ofp/transmogrify.h"
#include "ofp/message.h"
#include "ofp/flowmod.h"
#include "ofp/portstatus.h"
#include "ofp/experimenter.h"
#include "ofp/instructions.h"
#include "ofp/instructionrange.h"
#include "ofp/originalmatch.h"

namespace ofp { // <namespace ofp>

Transmogrify::Transmogrify(Message *message) : buf_{message->buf_} {}

void Transmogrify::normalize()
{
    assert(buf_.size() >= sizeof(Header));

    // Caution! Many magic numbers ahead...
    Header *hdr = header();

    // Translate type of message from earlier version into latest enum.
    OFPType type =
        Header::translateType(hdr->version(), hdr->type(), OFP_VERSION_4);
    if (type == OFPT_UNSUPPORTED) {
        log::info("Unsupported type for protocol version:", hdr->type());
    }
    hdr->setType(type);

    if (hdr->version() == OFP_VERSION_1) {
        if (hdr->type() == FlowMod::type()) {
            normalizeFlowModV1();
        } else if (hdr->type() == PortStatus::type()) {
            normalizePortStatusV1();
        } else if (hdr->type() == Experimenter::type()) {
            normalizeExperimenterV1();
        }
    }
}

void Transmogrify::normalizeFlowModV1()
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
    assert(origSize >= 72);

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
    // We need to insert 64 bytes + optionally 8 bytes.

    UInt16 actLen = UInt16_narrow_cast(origSize - 72);
    size_t needed = 64;
    if (actLen > 0) {
        needed += 8;
    }

    log::debug("TRANSMOGRIFY0: size=", buf_.size());
    // insertUninitialized() may move memory.
    buf_.insertUninitialized(pkt + 48, needed);

    log::debug("TRANSMOGRIFY0.a: size=", buf_.size());
    
    // This insertion may move memory; update pkt ptr just in case.
    // Copy in StandardMatch.

    pkt = buf_.mutableData();
    std::memcpy(pkt + 48, &stdMatch, sizeof(stdMatch));

    log::debug("TRANSMOGRIFY1: ", RawDataToHex(buf_.data(), buf_.size()));

    if (actLen > 0) {
        // Normalize actions may move memory.
        int delta = normActionsV1orV2(ActionRange{ByteRange{pkt + 144, actLen}}, stdMatch.nw_proto);

        pkt = buf_.mutableData();
        detail::InstructionHeaderWithPadding insHead{
            OFPIT_APPLY_ACTIONS, actLen + 8 + delta};
        std::memcpy(pkt + 136, &insHead, sizeof(insHead));
    }

    // Update header length. N.B. Make sure we use current header ptr.
    header()->setLength(UInt16_narrow_cast(buf_.size()));


    log::debug("TRANSMOGRIFY2: ", RawDataToHex(buf_.data(), buf_.size()));
}

void Transmogrify::normalizePortStatusV1()
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

void Transmogrify::normalizeExperimenterV1()
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
    header()->setLength(UInt16_narrow_cast(buf_.size()));
}

int Transmogrify::normInstructionsV1orV2(const InstructionRange &instr, UInt8 ipProto)
{
	// Return change in length of instructions list.
    return 0; 
}

int Transmogrify::normActionsV1orV2(const ActionRange &actions, UInt8 ipProto)
{
    using namespace deprecated;

    // Return change in length of action list.
    int lengthChange = 0;

    ActionIterator iter = actions.begin();
    ActionIterator iterEnd = actions.end();

    while (iter < iterEnd) {
        ActionType actType = iter.type();
        UInt16 type = actType.type();

        if ((type >= UInt16_cast(v1::OFPAT_SET_VLAN_VID) && type <= UInt16_cast(v1::OFPAT_ENQUEUE)) ||
             type == UInt16_cast(v2::OFPAT_SET_MPLS_LABEL) || type == UInt16_cast(v2::OFPAT_SET_MPLS_TC)) {
            lengthChange += normActionV1orV2(type, &iter, &iterEnd, ipProto);
        }
        ++iter;
    }

    return lengthChange;
}

int Transmogrify::normActionV1orV2(UInt16 type, ActionIterator *iter, ActionIterator *iterEnd, UInt8 ipProto)
{
    using namespace deprecated;

    UInt8 version = header()->version();

    if (version == OFP_VERSION_1) {
        // Normalize action type code to the V2+ schema.
        if (type == UInt16_cast(v1::OFPAT_STRIP_VLAN)) {
            type = OFPAT_STRIP_VLAN_V1;
        } else if (type == UInt16_cast(v1::OFPAT_ENQUEUE)) {
            type = OFPAT_ENQUEUE_V1;
        } else if (type >= UInt16_cast(v1::OFPAT_SET_DL_SRC) && type <= UInt16_cast(v1::OFPAT_SET_NW_TOS)) {
            --type;
        }
    }

    int lengthChange = 0;
    switch (type)
    {
        case v2::OFPAT_SET_VLAN_VID:
            lengthChange += normSetField<OFB_VLAN_VID>(iter, iterEnd);
            break;
        case v2::OFPAT_SET_VLAN_PCP:
            lengthChange += normSetField<OFB_VLAN_PCP>(iter, iterEnd);
            break;
        case v2::OFPAT_SET_DL_SRC:
            lengthChange += normSetField<OFB_ETH_SRC>(iter, iterEnd);
            break;
        case v2::OFPAT_SET_DL_DST:
            lengthChange += normSetField<OFB_ETH_DST>(iter, iterEnd);
            break;
        case v2::OFPAT_SET_NW_SRC:
            lengthChange += normSetField<OFB_IPV4_SRC>(iter, iterEnd);
            break;
        case v2::OFPAT_SET_NW_DST:
            lengthChange += normSetField<OFB_IPV4_DST>(iter, iterEnd);
            break;
        case v2::OFPAT_SET_NW_TOS:
            lengthChange += normSetField<OFB_IP_DSCP>(iter, iterEnd);
            break;
        case v2::OFPAT_SET_NW_ECN:
            lengthChange += normSetField<OFB_IP_ECN>(iter, iterEnd);
            break;
        case v2::OFPAT_SET_TP_SRC:
            if (ipProto == TCP) {
                lengthChange += normSetField<OFB_TCP_SRC>(iter, iterEnd);
            } else if (ipProto == UDP) {
                lengthChange += normSetField<OFB_UDP_SRC>(iter, iterEnd);
            } else if (ipProto == ICMP) {
                lengthChange += normSetField<OFB_ICMPV4_TYPE>(iter, iterEnd);
            } else {
                log::info("OFPAT_SET_TP_DST: Unknown proto", ipProto);
            }
            break;
        case v2::OFPAT_SET_TP_DST:
            if (ipProto == TCP) {
                lengthChange += normSetField<OFB_TCP_DST>(iter, iterEnd);
            } else if (ipProto == UDP) {
                lengthChange += normSetField<OFB_UDP_DST>(iter, iterEnd);
            } else if (ipProto == ICMP) {
                lengthChange += normSetField<OFB_ICMPV4_CODE>(iter, iterEnd);
            } else {
                log::info("OFPAT_SET_TP_DST: Unknown proto", ipProto);
            }
            break;
            
    }

    return lengthChange;
}


} // </namespace ofp>
