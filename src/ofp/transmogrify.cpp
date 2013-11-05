//  ===== ---- ofp/transmogrify.cpp ------------------------*- C++ -*- =====  //
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
/// \brief Implements Transmogrify class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/transmogrify.h"
#include "ofp/message.h"
#include "ofp/flowmod.h"
#include "ofp/portstatus.h"
#include "ofp/experimenter.h"
#include "ofp/packetout.h"
#include "ofp/portmod.h"
#include "ofp/flowremoved.h"
#include "ofp/multipartrequest.h"
#include "ofp/multipartreply.h"
#include "ofp/instructions.h"
#include "ofp/instructionrange.h"
#include "ofp/originalmatch.h"
#include "ofp/actions.h"

using namespace ofp;

using deprecated::OriginalMatch;
using deprecated::StandardMatch;

Transmogrify::Transmogrify(Message *message) : buf_(message->buf_) {}

void Transmogrify::normalize() {
  assert(buf_.size() >= sizeof(Header));

  Header *hdr = header();

  // Translate type of message from earlier version into latest enum.
  OFPType type =
      Header::translateType(hdr->version(), hdr->type(), OFP_VERSION_4);
  if (type == OFPT_UNSUPPORTED) {
    log::info("Unsupported type for protocol version:", hdr->type());
  }
  hdr->setType(type);

  if (hdr->version() == OFP_VERSION_1) {
    if (type == FlowMod::type()) {
      normalizeFlowModV1();
    } else if (type == PortStatus::type()) {
      normalizePortStatusV1();
    } else if (type == Experimenter::type()) {
      normalizeExperimenterV1();
    } else if (type == PacketOut::type()) {
      normalizePacketOutV1();
    } else if (type == PortMod::type()) {
      normalizePortModV1();
    } else if (type == FlowRemoved::type()) {
      normalizeFlowRemovedV1();
    } else if (type == MultipartRequest::type()) {
      normalizeMultipartRequestV1();
    } else if (type == MultipartReply::type()) {
      normalizeMultipartReplyV1();
    }
  }
}

void Transmogrify::normalizeFlowModV1() {
  Header *hdr = header();

  if (hdr->length() < 72) {
    log::info("FlowMod v1 message is too short.", hdr->length());
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  // Caution! Many magic numbers ahead...
  //
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

  // Convert port number from 16 to 32 bits.
  Big32 out_port = normPortNumberV1(pkt + 68);

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

  // insertUninitialized() may move memory.
  buf_.insertUninitialized(pkt + 48, needed);

  // This insertion may move memory; update pkt ptr just in case.
  // Copy in StandardMatch.

  pkt = buf_.mutableData();
  std::memcpy(pkt + 48, &stdMatch, sizeof(stdMatch));

  if (actLen > 0) {
    // Normalize actions may move memory.
    int delta = normActionsV1orV2(ActionRange{ByteRange{pkt + 144, actLen}},
                                  stdMatch.nw_proto);

    pkt = buf_.mutableData();
    detail::InstructionHeaderWithPadding insHead{
        OFPIT_APPLY_ACTIONS, UInt16_narrow_cast(actLen + 8 + delta)};
    std::memcpy(pkt + 136, &insHead, sizeof(insHead));
  }

  // Update header length. N.B. Make sure we use current header ptr.
  header()->setLength(UInt16_narrow_cast(buf_.size()));
}

void Transmogrify::normalizePortStatusV1() {
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

void Transmogrify::normalizeExperimenterV1() {
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

void Transmogrify::normalizePacketOutV1() {
  Header *hdr = header();

  if (hdr->length() < 16) {
    log::info("PacketOut v1 message is too short.", hdr->length());
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  // 1. Convert inPort from 16 to 32 bits.
  // 2. Insert 6 pad bytes after 32 bit inPort.
  // 3. Normalize actions, if present.

  UInt8 *pkt = buf_.mutableData();
  Big32 inPort = normPortNumberV1(pkt + 12);
  UInt16 actionLen = *reinterpret_cast<Big16 *>(pkt + 14);

  // Insert 8 bytes at position 12 (2 for port, 6 pad).
  buf_.insertUninitialized(pkt + 12, 8);
  pkt = buf_.mutableData();

  // Replace inPort. Set pad bytes to zero.
  std::memcpy(pkt + 12, &inPort, sizeof(inPort));
  std::memset(pkt + 18, 0, 6);

  // Normalize actions.
  UInt16 actLen = actionLen;
  if (actLen > 0) {
    // N.B. Later versions break out the different types of TCP, UDP, ICMP,
    // and SCTP ports. Unfortunately, there is not enough information in a
    // a PacketOut message to divine the IP protocol type. In this case, we
    // do not normalize the OFPAT_SET_TP_SRC and OFPAT_SET_TP_DST actions.
    // To do this, we pass an ipProto value of 0.

    ActionRange actions{ByteRange{pkt + 24, actLen}};
    int lengthChange = normActionsV1orV2(actions, 0);
    pkt = buf_.mutableData();

    actionLen += lengthChange;
  }

  Big16 bigLen = actionLen;
  std::memcpy(pkt + 16, &bigLen, sizeof(bigLen));

  // Update header length. N.B. Make sure we use current header ptr.
  header()->setLength(UInt16_narrow_cast(buf_.size()));
}

void Transmogrify::normalizePortModV1() {
  Header *hdr = header();

  if (hdr->length() < 32) {
    log::info("PortMod v1 message is too short.", hdr->length());
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  UInt8 *pkt = buf_.mutableData();
  Big32 inPort = normPortNumberV1(pkt + 8);

  // Insert 8 bytes at position 16.
  buf_.insertUninitialized(pkt + 16, 8);

  // Move hwAddr field 6 bytes.
  pkt = buf_.mutableData();
  std::memcpy(pkt + 16, pkt + 10, 6);

  // Set 32-bit port number.
  std::memcpy(pkt + 8, &inPort, sizeof(inPort));

  // Set padding bytes to zero.
  std::memset(pkt + 12, 0, 4);
  std::memset(pkt + 22, 0, 2);
}

void Transmogrify::normalizeFlowRemovedV1() {
  Header *hdr = header();

  if (hdr->length() != 88) {
    log::info("FlowRemoved v1 message is wrong length.", hdr->length());
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  // To convert a v1 FlowRemoved into v2:
  //
  // 1. Copy OriginalMatch into StandardMatch.
  // 2. Shift bytes in rest of packet upward.
  // 3. Move StandardMatch after packet data.
  // 4. Update header length.
  //
  // Note: Size changes from 88 to 136.

  UInt8 *pkt = buf_.mutableData();
  assert(buf_.size() == 88);

  OriginalMatch *origMatch =
      reinterpret_cast<OriginalMatch *>(pkt + sizeof(Header));

  StandardMatch stdMatch{*origMatch};

  std::memcpy(pkt + 8, pkt + 48, 40);
  buf_.insertUninitialized(pkt + 88, 136 - 88);

  pkt = buf_.mutableData();
  std::memcpy(pkt + 48, &stdMatch, sizeof(stdMatch));

  header()->setLength(UInt16_narrow_cast(buf_.size()));
}

void Transmogrify::normalizeMultipartRequestV1() {
  Header *hdr = header();
  if (hdr->length() < sizeof(MultipartRequest)) {
    log::info("MultipartRequest v1 message is too short.", hdr->length());
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  const MultipartRequest *multipartReq =
      reinterpret_cast<const MultipartRequest *>(hdr);

  OFPMultipartType reqType = multipartReq->requestType();
  if (reqType == OFPMP_FLOW || reqType == OFPMP_AGGREGATE) {
    // Both requests have the same structure.
    normalizeMPFlowRequestV1();
  }

  header()->setLength(UInt16_narrow_cast(buf_.size()));
}


void Transmogrify::normalizeMultipartReplyV1()
{
  Header *hdr = header();
  if (hdr->length() < sizeof(MultipartReply)) {
    log::info("MultipartReply v1 message is too short.", hdr->length());
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  const MultipartReply *multipartReply =
      reinterpret_cast<const MultipartReply *>(hdr);

  OFPMultipartType replyType = multipartReply->replyType();
  size_t offset = sizeof(MultipartReply);

  if (replyType == OFPMP_FLOW) {
    while (offset < buf_.size()) 
      normalizeMPFlowReplyV1(&offset);
    assert(offset == buf_.size());
  }

  header()->setLength(UInt16_narrow_cast(buf_.size()));
}

void Transmogrify::normalizeMPFlowRequestV1() {
  // Check length of packet.
  if (buf_.size() != sizeof(MultipartRequest) + 44) {
    log::info("MultipartRequest v1 OFPMP_FLOW is wrong length.", buf_.size());
    header()->setType(OFPT_UNSUPPORTED);
    return;
  }

  // Increase buf size from (16+44) to (16+120).
  buf_.addUninitialized(76);

  UInt8 *ptr = buf_.mutableData() + sizeof(MultipartRequest);
  OriginalMatch *origMatch = reinterpret_cast<OriginalMatch *>(ptr);
  StandardMatch stdMatch{*origMatch};

  // Coerce 16-bit port number to 32-bits.
  Big32 outPort = normPortNumberV1(ptr + sizeof(OriginalMatch) + 2);

  // Shift tableId and pad1 up (2 bytes).
  std::memcpy(ptr, ptr + sizeof(OriginalMatch), 2);
  // Add 2 bytes of pad.
  std::memset(ptr + 2, 0, 2);
  // Add 32-bit outPort.
  std::memcpy(ptr + 4, &outPort, sizeof(outPort));
  // Zero next 24 bytes.
  std::memset(ptr + 8, 0, 24);
  // Add stdMatch.
  std::memcpy(ptr + 32, &stdMatch, sizeof(stdMatch));

  assert(buf_.size() == sizeof(MultipartRequest) + 120);
}

void Transmogrify::normalizeMPFlowReplyV1(size_t *start) {
  // Normalize the FlowStatsReply (ptr, length)
  size_t offset = *start;
  UInt8 *ptr = buf_.mutableData() + offset;
  UInt16 length = *reinterpret_cast<Big16 *>(ptr);

  if (length < 88) {
    log::info("FlowStatsReply v1 is too short.", length);
    *start = buf_.size();
    return;
  }

  OriginalMatch *origMatch = reinterpret_cast<OriginalMatch *>(ptr + 4);
  StandardMatch stdMatch{*origMatch};

  // Shift all fields above actions up. Must use memmove due to overlap.
  std::memmove(ptr + 4, ptr + 44, 44);

  printf("size: %lu, %d\n", buf_.size(), length);
  log::debug(" ", buf_);
  
  // Need at least 48 more bytes (136 - 88). If there's an action list, we need 
  // 8 more bytes for the instruction header.
  UInt16 actLen = UInt16_narrow_cast(length - 88);
  int needed = 48;
  if (actLen > 0) {
    needed += 8;
  }

  buf_.insertUninitialized(ptr + 48, needed);

  // Copy in stdMatch.
  ptr = buf_.mutableData() + offset;
  std::memcpy(ptr + 48, &stdMatch, sizeof(stdMatch));

  // TODO: see common code in FlowModv1.
  if (actLen > 0) {
    // Normalize actions may move memory.
    int delta = normActionsV1orV2(ActionRange{ByteRange{ptr + 144, actLen}},
                                  stdMatch.nw_proto);

    ptr = buf_.mutableData() + offset;
    detail::InstructionHeaderWithPadding insHead{
        OFPIT_APPLY_ACTIONS, UInt16_narrow_cast(actLen + 8 + delta)};
    std::memcpy(ptr + 136, &insHead, sizeof(insHead));

    needed += delta;
  }

  // Update length.
  *reinterpret_cast<Big16 *>(ptr) = length + needed;
  *start += length + needed;
  
  log::debug("normalizeFlowReplyV1", buf_);
}

UInt32 Transmogrify::normPortNumberV1(const UInt8 *ptr) {
  const Big16 *port16 = reinterpret_cast<const Big16 *>(ptr);

  UInt32 port32 = *port16;
  if (port32 > 0xFF00U) {
    // Sign extend to 32-bits the "fake" ports.
    port32 |= 0xFFFF0000UL;
  }

  return port32;
}

int Transmogrify::normInstructionsV1orV2(const InstructionRange &instr,
                                         UInt8 ipProto) {
  // Return change in length of instructions list.
  return 0;
}

int Transmogrify::normActionsV1orV2(const ActionRange &actions, UInt8 ipProto) {
  using namespace deprecated;

  // Return change in length of action list.
  int lengthChange = 0;

  ActionIterator iter = actions.begin();
  ActionIterator iterEnd = actions.end();

  while (iter < iterEnd) {
    ActionType actType = iter.type();
    UInt16 type = actType.type();

    if (type <= UInt16_cast(v1::OFPAT_ENQUEUE) ||
        type == UInt16_cast(v2::OFPAT_SET_MPLS_LABEL) ||
        type == UInt16_cast(v2::OFPAT_SET_MPLS_TC)) {
      lengthChange += normActionV1orV2(type, &iter, &iterEnd, ipProto);
    }
    ++iter;
  }

  return lengthChange;
}

int Transmogrify::normActionV1orV2(UInt16 type, ActionIterator *iter,
                                   ActionIterator *iterEnd, UInt8 ipProto) {
  using namespace deprecated;

  UInt8 version = header()->version();

  if (version == OFP_VERSION_1) {
    // Normalize action type code to the V2+ schema.
    if (type == UInt16_cast(v1::OFPAT_STRIP_VLAN)) {
      type = OFPAT_STRIP_VLAN_V1;
    } else if (type == UInt16_cast(v1::OFPAT_ENQUEUE)) {
      type = OFPAT_ENQUEUE_V1;
    } else if (type >= UInt16_cast(v1::OFPAT_SET_DL_SRC) &&
               type <= UInt16_cast(v1::OFPAT_SET_NW_TOS)) {
      --type;
    }
  }

  int lengthChange = 0;
  switch (type) {
  case v1::OFPAT_OUTPUT:
    lengthChange += normOutput(iter, iterEnd);
    break;
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

int Transmogrify::normOutput(ActionIterator *iter, ActionIterator *iterEnd) {
  if (iter->type() != deprecated::AT_OUTPUT_V1::type())
    return 0;

  // Change port number from 16 to 32 bits.
  UInt32 port = normPortNumberV1(iter->valuePtr());
  UInt16 maxlen = *reinterpret_cast<const Big16 *>(iter->valuePtr() + 2);

  AT_OUTPUT output{port, maxlen};

  ptrdiff_t offset = buf_.offset(iter->data());
  buf_.insertUninitialized(iter->valuePtr(), 8);
  *iter = ActionIterator{buf_.data() + offset};
  *iterEnd = ActionIterator{buf_.end()};

  std::memcpy(RemoveConst_cast(iter->data()), &output, sizeof(output));

  assert(iter->type() == AT_OUTPUT::type());

  return 8;
}
