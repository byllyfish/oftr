#include "ofp/message.h"
#include "ofp/sys/connection.h"
#include "ofp/flowmod.h"
#include "ofp/instructions.h"
#include "ofp/originalmatch.h"

namespace ofp { // <namespace ofp>

Channel *Message::source() const
{
    return channel_;
}

/// \brief Modify the binary representation of the message to fit our standard
/// layout protocol interface. This method changes a version 1 FlowMod 
/// representation into the version 2+ FlowMod representation. It leaves other 
/// messages untouched. (This strategy assumes that the v1 and v2 protocol specs
/// will not change retroactively.)
///

void Message::transmogrify()
{
	// Caution! Many magic numbers ahead...

	using deprecated::OriginalMatch;
	using deprecated::StandardMatch;

	Header *hdr = header();

	if (hdr->type() == FlowMod::Type && hdr->version() == OFP_VERSION_1) {

		if (hdr->length() < 72) {
			log::info("FlowMod v1 message is too short.", hdr->length());
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

		OriginalMatch *origMatch = reinterpret_cast<OriginalMatch *>(pkt + sizeof(Header));
		log::debug("OriginalMatch", RawDataToHex(origMatch, sizeof(OriginalMatch)));

		StandardMatch stdMatch{*origMatch};
		UInt64 cookie_mask = ~0ULL;

		Big16 *out_port16 = reinterpret_cast<Big16*>(pkt + 68);
		UInt32 out_port32 = *out_port16;
		if (out_port32 > 0xFF00U) {
			// Sign extend to 32-bits the "fake" ports.
			out_port32 |= 0xFFFF0000U;
		}
		Big32  out_port = out_port32;

		std::memcpy(pkt + 8, pkt + 48, 8);  	// set cookie
		std::memcpy(pkt + 16, &cookie_mask, 8); // set cookie_mask
		std::memcpy(pkt + 24, pkt + 56, 12);    // set up to out_port
		std::memcpy(pkt + 36, &out_port, 4);	// set out_port as 32 bits
		std::memset(pkt + 40, 0, 4);			// set out_group
		std::memcpy(pkt + 44, pkt + 70, 2);		// set flags
		std::memset(pkt + 46, 0, 2);			// set 2 pad bytes

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
		detail::InstructionHeaderWithPadding insHead{InstructionType::IT_APPLY_ACTIONS, actLen};

		std::memcpy(pkt + 120, &insHead, sizeof(insHead));

		// Update header length. N.B. Make sure we use current header ptr.
		header()->setLength(UInt16_narrow_cast(size()));
	}
}

} // </namespace ofp>