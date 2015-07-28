// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/transmogrify.h"
#include "ofp/message.h"
#include "ofp/featuresreply.h"
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
#include "ofp/portstatsproperty.h"
#include "ofp/setasync.h"
#include "ofp/queuegetconfigreply.h"

using namespace ofp;

using deprecated::OriginalMatch;
using deprecated::StandardMatch;

// This is defined here instead of Transmogrify.h because of header
// dependencies.
Transmogrify::Transmogrify(Message *message) : buf_(message->buf_) {
}

void Transmogrify::normalize() {
  if (buf_.size() < sizeof(Header)) {
    log::fatal("Transmogrify::normalize called with invalid message");
  }

  Header *hdr = header();

  // Translate type of message from earlier version into latest enum.
  OFPType type =
      Header::translateType(hdr->version(), hdr->type(), OFP_VERSION_4);
  if (type == OFPT_UNSUPPORTED) {
    log::info("Unsupported type for protocol version:", hdr->type());
  }
  hdr->setType(type);

  log::debug("normalize", type);

  // If header length doesn't match buffer size, we have a problem.
  if (buf_.size() != hdr->length()) {
    log::info("Unexpected header length:", hdr->length());
    hdr->setLength(UInt16_narrow_cast(buf_.size()));
    hdr->setType(OFPT_UNSUPPORTED);
  }

  UInt8 version = hdr->version();
  if (version == OFP_VERSION_1) {
    if (type == FeaturesReply::type()) {
      normalizeFeaturesReplyV1();
    } else if (type == FlowMod::type()) {
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
    } else if (type == QueueGetConfigReply::type()) {
      normalizeQueueGetConfigReplyV1();
    }
  } else if (version == OFP_VERSION_2) {
    if (type == FeaturesReply::type()) {
      normalizeFeaturesReplyV2();
    } else if (type == PortStatus::type()) {
      normalizePortStatusV2();
    } else if (type == PortMod::type()) {
      normalizePortModV2();
    } else if (type == MultipartReply::type()) {
      normalizeMultipartReplyV2();
    } else if (type == QueueGetConfigReply::type()) {
      normalizeQueueGetConfigReplyV1();
    }
  } else if (version == OFP_VERSION_3) {
    if (type == FeaturesReply::type()) {
      normalizeFeaturesReplyV2();
    } else if (type == PortStatus::type()) {
      normalizePortStatusV2();
    } else if (type == PortMod::type()) {
      normalizePortModV2();
    } else if (type == MultipartReply::type()) {
      normalizeMultipartReplyV3();
    } else if (type == QueueGetConfigReply::type()) {
      normalizeQueueGetConfigReplyV2();
    }
  } else if (version == OFP_VERSION_4) {
    if (type == FeaturesReply::type()) {
      normalizeFeaturesReplyV2();
    } else if (type == PortStatus::type()) {
      normalizePortStatusV2();
    } else if (type == PortMod::type()) {
      normalizePortModV2();
    } else if (type == MultipartReply::type()) {
      normalizeMultipartReplyV4();
    } else if (type == SetAsync::type()) {
      normalizeSetAsyncV4();
    } else if (type == QueueGetConfigReply::type()) {
      normalizeQueueGetConfigReplyV2();
    }
  }

  assert(buf_.size() == header()->length());
}

void Transmogrify::normalizeFeaturesReplyV1() {
  using deprecated::PortV1;

  Header *hdr = header();

  // TODO(bfish): min length already checked?
  if (hdr->length() < sizeof(FeaturesReply)) {
    log::debug("Invalid FeaturesReply size.");
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  // Verify size of port list.
  size_t portListSize = hdr->length() - sizeof(FeaturesReply);
  if ((portListSize % sizeof(PortV1)) != 0) {
    log::debug("Invalid FeaturesReply port list size.");
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  // Normalize the port structures from V1 to normal size.
  size_t portCount = portListSize / sizeof(PortV1);
  UInt8 *pkt = buf_.mutableData();
  const PortV1 *portV1 =
      reinterpret_cast<const PortV1 *>(pkt + sizeof(FeaturesReply));

  PortList ports;
  for (size_t i = 0; i < portCount; ++i) {
    PortBuilder newPort{*portV1};
    ports.add(newPort);
    ++portV1;
  }

  if (ports.size() > 65535 - sizeof(FeaturesReply)) {
    log::debug("Invalid FeaturesReply new port list size too big");
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  // Copy new port list into packet.
  buf_.addUninitialized(ports.size() - portListSize);
  pkt = buf_.mutableData();
  assert(buf_.size() == sizeof(FeaturesReply) + ports.size());
  std::memcpy(pkt + sizeof(FeaturesReply), ports.data(), ports.size());

  // Update header length. N.B. Make sure we use current header ptr.
  // TODO(bfish): Put header fix up at end of calling method?
  header()->setLength(UInt16_narrow_cast(buf_.size()));
}

void Transmogrify::normalizeFeaturesReplyV2() {
  using deprecated::PortV2;

  Header *hdr = header();

  if (hdr->length() < sizeof(FeaturesReply)) {
    log::debug("Invalid FeaturesReply size.");
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  // Verify size of port list.
  size_t portListSize = hdr->length() - sizeof(FeaturesReply);
  if ((portListSize % sizeof(PortV2)) != 0) {
    log::debug("Invalid FeaturesReply port list size.");
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  // Normalize the port structures from V2 to normal size.
  size_t portCount = portListSize / sizeof(PortV2);
  UInt8 *pkt = buf_.mutableData();
  const PortV2 *portV2 =
      reinterpret_cast<const PortV2 *>(pkt + sizeof(FeaturesReply));

  PortList ports;
  for (size_t i = 0; i < portCount; ++i) {
    PortBuilder newPort{*portV2};
    ports.add(newPort);
    ++portV2;
  }

  if (ports.size() > 65535 - sizeof(FeaturesReply)) {
    log::debug("Invalid FeaturesReply new port list size.");
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  // Copy new port list into packet.
  buf_.addUninitialized(ports.size() - portListSize);
  pkt = buf_.mutableData();
  assert(buf_.size() == sizeof(FeaturesReply) + ports.size());
  std::memcpy(pkt + sizeof(FeaturesReply), ports.data(), ports.size());

  // Update header length. N.B. Make sure we use current header ptr.
  header()->setLength(UInt16_narrow_cast(buf_.size()));
}

void Transmogrify::normalizeFlowModV1() {
  Header *hdr = header();

  if (hdr->length() < 72) {
    log::info("FlowMod v1 message is too short.", hdr->length());
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  if (buf_.size() > 65535 - 72) {
    log::info("FlowMod v1 message is too long.", hdr->length());
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

  std::memcpy(pkt + 8, pkt + 48, 8);       // set cookie
  std::memcpy(pkt + 16, &cookie_mask, 8);  // set cookie_mask
  std::memcpy(pkt + 24, pkt + 56, 12);     // set up to out_port
  std::memcpy(pkt + 36, &out_port, 4);     // set out_port as 32 bits
  std::memset(pkt + 40, 0, 4);             // set out_group
  std::memcpy(pkt + 44, pkt + 70, 2);      // set flags
  std::memset(pkt + 46, 0, 2);             // set 2 pad bytes

  // Current position is pkt + 48.
  // StandardMatch size is 88.
  // Actions start at pkt + 72.
  // We need to convert this actionlist into an ApplyActions instruction.
  //   1. Insert type, length and 4 bytes (8 bytes total)
  // We need to insert 64 bytes + optionally 8 bytes.

  UInt16 actLen = UInt16_narrow_cast(origSize - 72);
  if ((actLen % 8) != 0) {
    log::info("FlowMod v1 actions size not aligned", actLen);
    return;
  }

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
    ActionRange actions{ByteRange{pkt + 144, actLen}};

    // Validate actions.
    OFPErrorCode error;
    Validation context{nullptr, &error};
    if (!actions.validateInput(&context)) {
      log::warning("Invalid actions in FlowMod v1");
      header()->setLength(UInt16_narrow_cast(buf_.size()));
      return;
    }

    // Normalize actions may move memory.
    int delta = normActionsV1orV2(actions, stdMatch.nw_proto);

    pkt = buf_.mutableData();
    detail::InstructionHeaderWithPadding insHead{
        OFPIT_APPLY_ACTIONS,
        UInt16_narrow_cast(Unsigned_cast(actLen + 8 + delta))};
    std::memcpy(pkt + 136, &insHead, sizeof(insHead));
  }

  // Update header length. N.B. Make sure we use current header ptr.
  header()->setLength(UInt16_narrow_cast(buf_.size()));
}

void Transmogrify::normalizePortStatusV1() {
  using deprecated::PortV1;
  const size_t PortStatusSize = 16;

  Header *hdr = header();

  if (hdr->length() != PortStatusSize + sizeof(PortV1)) {
    log::info("PortStatus v1 message is wrong size.", hdr->length());
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  UInt8 *pkt = buf_.mutableData();

  PortV1 *portV1 = reinterpret_cast<PortV1 *>(pkt + PortStatusSize);
  PortBuilder portBuilder{*portV1};

  buf_.addUninitialized(Port::DefaultSizeEthernet - sizeof(PortV1));

  // The insertion may move memory; update `pkt` just in case.
  pkt = buf_.mutableData();
  portBuilder.copyTo(pkt + PortStatusSize);

  assert(buf_.size() == PortStatusSize + Port::DefaultSizeEthernet);
  header()->setLength(UInt16_narrow_cast(buf_.size()));
}

void Transmogrify::normalizePortStatusV2() {
  using deprecated::PortV2;
  const size_t PortStatusSize = 16;

  Header *hdr = header();

  if (hdr->length() != PortStatusSize + sizeof(PortV2)) {
    log::info("PortStatus v2 message is wrong size.", hdr->length());
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  UInt8 *pkt = buf_.mutableData();

  PortV2 *portV2 = reinterpret_cast<PortV2 *>(pkt + PortStatusSize);
  PortBuilder portBuilder{*portV2};

  buf_.addUninitialized(Port::DefaultSizeEthernet - sizeof(PortV2));

  // The insertion may move memory; update `pkt` just in case.
  pkt = buf_.mutableData();
  portBuilder.copyTo(pkt + PortStatusSize);

  assert(buf_.size() == PortStatusSize + Port::DefaultSizeEthernet);
  header()->setLength(UInt16_narrow_cast(buf_.size()));
}

void Transmogrify::normalizeExperimenterV1() {
  Header *hdr = header();

  if (hdr->length() < 12) {
    log::info("Experimenter v1 message is too short.", hdr->length());
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  if (hdr->length() > 65535 - 4) {
    log::info("Experimenter v1 message is too long.", hdr->length());
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

  if (buf_.size() > 65535 - 8) {
    log::info("PacketOut v1 message is too long.", hdr->length());
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  // 1. Convert inPort from 16 to 32 bits.
  // 2. Insert 6 pad bytes after 32 bit inPort.
  // 3. Normalize actions, if present.

  UInt8 *pkt = buf_.mutableData();
  Big32 inPort = normPortNumberV1(pkt + 12);
  UInt16 actionLen = *Big16_cast(pkt + 14);

  if ((actionLen % 8) != 0) {
    log::warning("PacketOut v1 message has invalid action len", actionLen);
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  } else if (16u + actionLen > buf_.size()) {
    log::warning("PacketOut v1 message action len overruns end", actionLen);
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

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

    OFPErrorCode error;
    Validation context{nullptr, &error};
    if (!actions.validateInput(&context)) {
      log::warning("Invalid actions in PacketOut v1");
      header()->setLength(UInt16_narrow_cast(buf_.size()));
      return;
    }

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

  // Convert advertise into a property.
  UInt32 advertise = *Big32_cast(pkt + 32);
  *Big32_cast(pkt + 32) = 0x00000008;
  *Big32_cast(pkt + 36) = OFPPortFeaturesFlagsConvertFromV1(advertise);

  log::info("normalizePortModV1", buf_);

  // Update header length.
  header()->setLength(UInt16_narrow_cast(buf_.size()));
}

void Transmogrify::normalizePortModV2() {
  Header *hdr = header();

  if (hdr->length() < 40) {
    log::info("PortMod message is too short.", hdr->length());
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  log::info("normalizePortModV2", buf_);

  // Convert advertise into a property.
  UInt8 *pkt = buf_.mutableData();
  UInt32 advertise = *Big32_cast(pkt + 32);
  *Big32_cast(pkt + 32) = 0x00000008;
  *Big32_cast(pkt + 36) = advertise;

  // No change in length.
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
  if (hdr->length() < MultipartRequest::UnpaddedSizeVersion1) {
    log::info("MultipartRequest v1 message is too short.", hdr->length());
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  // Insert 4 bytes of padding.
  Padding<4> pad;
  buf_.insert(BytePtr(hdr) + MultipartRequest::UnpaddedSizeVersion1, &pad,
              sizeof(pad));

  const MultipartRequest *multipartReq =
      reinterpret_cast<const MultipartRequest *>(header());

  OFPMultipartType reqType = multipartReq->requestType();
  if (reqType == OFPMP_FLOW || reqType == OFPMP_AGGREGATE) {
    // Both requests have the same structure.
    normalizeMPFlowRequestV1();
  } else if (reqType == OFPMP_PORT_STATS || reqType == OFPMP_QUEUE) {
    // Both requests have a port number that needs to be converted as the
    // first field.
    normalizeMPPortStatsRequestV1();
  }

  header()->setLength(UInt16_narrow_cast(buf_.size()));
}

void Transmogrify::normalizeMultipartReplyV1() {
  Header *hdr = header();
  if (hdr->length() < MultipartReply::UnpaddedSizeVersion1) {
    log::info("MultipartReply v1 message is too short.", hdr->length());
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  // Insert 4 bytes of padding.
  Padding<4> pad;
  buf_.insert(BytePtr(hdr) + MultipartReply::UnpaddedSizeVersion1, &pad,
              sizeof(pad));

  const MultipartReply *multipartReply =
      reinterpret_cast<const MultipartReply *>(header());

  OFPMultipartType replyType = multipartReply->replyType();
  size_t offset = sizeof(MultipartReply);

  if (replyType == OFPMP_FLOW) {
    while (offset < buf_.size())
      normalizeMPFlowReplyV1(&offset);
    assert(offset == buf_.size());
  } else if (replyType == OFPMP_PORT_STATS) {
    while (offset < buf_.size())
      normalizeMPPortStatsReplyV1(&offset);
    assert(offset == buf_.size());
  } else if (replyType == OFPMP_QUEUE) {
    while (offset < buf_.size())
      normalizeMPPortOrQueueStatsReplyV1(&offset, 32);
    assert(offset == buf_.size());
  } else if (replyType == OFPMP_PORT_DESC) {
    normalizeMPPortDescReplyV1();
  }

  header()->setLength(UInt16_narrow_cast(buf_.size()));
}

void Transmogrify::normalizeMultipartReplyV2() {
  Header *hdr = header();
  if (hdr->length() < sizeof(MultipartReply)) {
    log::info("MultipartReply v2 message is too short.", hdr->length());
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  const MultipartReply *multipartReply =
      reinterpret_cast<const MultipartReply *>(hdr);

  OFPMultipartType replyType = multipartReply->replyType();
  size_t offset = sizeof(MultipartReply);

  if (replyType == OFPMP_PORT_STATS) {
    while (offset < buf_.size())
      normalizeMPPortStatsReplyV2(&offset);
    assert(offset == buf_.size());
  } else if (replyType == OFPMP_PORT_DESC) {
    normalizeMPPortDescReplyV4();
  }

  header()->setLength(UInt16_narrow_cast(buf_.size()));
}

void Transmogrify::normalizeMultipartReplyV3() {
  Header *hdr = header();
  if (hdr->length() < sizeof(MultipartReply)) {
    log::info("MultipartReply v3 message is too short.", hdr->length());
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  const MultipartReply *multipartReply =
      reinterpret_cast<const MultipartReply *>(hdr);

  OFPMultipartType replyType = multipartReply->replyType();
  size_t offset = sizeof(MultipartReply);

  if (replyType == OFPMP_PORT_STATS) {
    while (offset < buf_.size())
      normalizeMPPortStatsReplyV2(&offset);
    assert(offset == buf_.size());
  } else if (replyType == OFPMP_QUEUE) {
    while (offset < buf_.size())
      normalizeMPPortOrQueueStatsReplyV3(&offset, 32);
    assert(offset == buf_.size());
  } else if (replyType == OFPMP_PORT_DESC) {
    normalizeMPPortDescReplyV4();
  }

  header()->setLength(UInt16_narrow_cast(buf_.size()));
}

void Transmogrify::normalizeMultipartReplyV4() {
  Header *hdr = header();
  if (hdr->length() < sizeof(MultipartReply)) {
    log::info("MultipartReply v4 message is too short.", hdr->length());
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  const MultipartReply *multipartReply =
      reinterpret_cast<const MultipartReply *>(hdr);

  OFPMultipartType replyType = multipartReply->replyType();
  size_t offset = sizeof(MultipartReply);

  if (replyType == OFPMP_PORT_STATS) {
    while (offset < buf_.size())
      normalizeMPPortStatsReplyV4(&offset);
    assert(offset == buf_.size());
  } else if (replyType == OFPMP_PORT_DESC) {
    normalizeMPPortDescReplyV4();
  } else if (replyType == OFPMP_TABLE) {
    while (offset < buf_.size())
      normalizeMPTableStatsReplyV4(&offset);
    assert(offset == buf_.size());
  }

  header()->setLength(UInt16_narrow_cast(buf_.size()));
}

void Transmogrify::normalizeSetAsyncV4() {
  Header *hdr = header();
  if (hdr->length() != 32) {
    log::info("SetAsync v4 message is wrong length.", hdr->length());
    hdr->setType(OFPT_UNSUPPORTED);
    return;
  }

  // New size is 56 bytes - up from 32 bytes.
  buf_.addUninitialized(24);

  UInt8 *ptr = buf_.mutableData() + sizeof(Header);

  Big32 masks[6];
  std::memcpy(masks, ptr, sizeof(masks));
  std::swap(masks[0], masks[1]);
  std::swap(masks[2], masks[3]);
  std::swap(masks[4], masks[5]);

  struct Prop {
    Big16 type;
    Big16 len;
    Big32 value;
  };

  Prop *out = reinterpret_cast<Prop *>(ptr);
  for (size_t i = 0; i < ArrayLength(masks); ++i) {
    out->type = UInt16_narrow_cast(i);
    out->len = 8;
    out->value = masks[i];
    ++out;
  }

  header()->setLength(UInt16_narrow_cast(buf_.size()));
}

void Transmogrify::normalizeQueueGetConfigReplyV1() {
  Header *hdr = header();

  size_t length = hdr->length();
  if (length < sizeof(QueueGetConfigReply)) {
    log::info("QueueGetConfigReply v1 is too short.", length);
    header()->setType(OFPT_UNSUPPORTED);
    return;
  }

  // Convert port number to 32-bits.
  UInt8 *ptr = buf_.mutableData() + sizeof(Header);
  PortNumber port = PortNumber::fromV1(*Big16_cast(ptr));
  std::memcpy(ptr, &port, sizeof(port));

  // Convert QueueV1 to Queue.
  ByteRange data = SafeByteRange(buf_.mutableData(), buf_.size(), sizeof(QueueGetConfigReply));
  deprecated::QueueV1Range queues{data};

  Validation context;
  if (!queues.validateInput(&context)) {
    log::warning("Invalid queues in QueueGetConfigReply v1");
    return;
  }

  QueueList newQueues;
  for (auto &queue : queues) {
    QueueBuilder newQueue{queue};
    newQueues.add(newQueue);
  }

  // When converting to the regular `Queue` structure, we may exceed the max
  // message size of 65535.
  if (newQueues.size() > 65535 - sizeof(QueueGetConfigReply)) {
    log::warning("QueueGetConfigReply v1 contains too many queues.", length);
    header()->setType(OFPT_UNSUPPORTED);
    return;
  }

  buf_.replace(data.begin(), data.end(), newQueues.data(), newQueues.size());

  header()->setLength(UInt16_narrow_cast(buf_.size()));
}

void Transmogrify::normalizeQueueGetConfigReplyV2() {
  // If length of message is not padded to a multiple of 8, pad it out.
  Header *hdr = header();

  size_t length = hdr->length();
  if ((length % 8) == 0) {
    return;
  }

  size_t newLength = PadLength(length);
  buf_.addZeros(newLength - length);

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

  if (offset + length > buf_.size()) {
    log::warning("FlowStatsReply v1 length overruns end");
    *start = buf_.size();
    return;
  }

  log::debug("normalizeMPFlowReplyv1 ", ByteRange{ptr, length});

  if (length < 88) {
    log::info("FlowStatsReply v1 is too short.", length);
    *start = buf_.size();
    return;
  }

  OriginalMatch *origMatch = reinterpret_cast<OriginalMatch *>(ptr + 4);
  StandardMatch stdMatch{*origMatch};

  // Shift all fields above actions up. Must use memmove due to overlap.
  std::memmove(ptr + 4, ptr + 44, 44);

  // Need at least 48 more bytes (136 - 88). If there's an action list, we need
  // 8 more bytes for the instruction header.
  UInt16 actLen = UInt16_narrow_cast(length - 88u);
  if ((actLen % 8) != 0) {
    log::info("FlowStatsReply v1 action length misaligned", actLen);
    *start = buf_.size();
    return;
  }

  int needed = 48;
  if (actLen > 0) {
    needed += 8;
  }

  buf_.insertUninitialized(ptr + 48, Unsigned_cast(needed));

  // Copy in stdMatch.
  ptr = buf_.mutableData() + offset;
  std::memcpy(ptr + 48, &stdMatch, sizeof(stdMatch));

  // TODO(bfish): see common code in FlowModv1.
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
  *reinterpret_cast<Big16 *>(ptr) = UInt16_narrow_cast(length + needed);
  *start = Unsigned_cast(Signed_cast(*start) + length + needed);

  log::debug("normalizeFlowReplyV1", buf_);
}

void Transmogrify::normalizeMPTableStatsReplyV4(size_t *start) {
  // Normalize the TableStatsReply V4 to look like a V1 message.
  size_t offset = *start;
  UInt8 *ptr = buf_.mutableData() + offset;
  size_t length = buf_.size();

  if (length - offset < 24) {
    log::info("TableStatsReply v4 is wrong length:", length);
    *start = buf_.size();
    return;
  }

  buf_.insertZeros(ptr + 4, 40);

  *start += 64;
}

void Transmogrify::normalizeMPPortOrQueueStatsReplyV1(size_t *start,
                                                      size_t len) {
  // Normalize the PortStatsReply V1 to look like a V4+ message.
  size_t offset = *start;
  size_t remaining = buf_.size() - offset;

  if (remaining < len) {
    *start = buf_.size();
    return;
  }

  UInt8 *ptr = buf_.mutableData() + offset;

  // Change port number from 16-bits to 32-bits.
  Big32 *port = reinterpret_cast<Big32 *>(ptr);
  *port = normPortNumberV1(ptr);

  // Insert an additional 8-bytes for timestamp.
  buf_.insertZeros(ptr + len, 8);
  *start += len + 8;
}

void Transmogrify::normalizeMPPortStatsRequestV1() {
  // Check length of packet.
  if (buf_.size() != sizeof(MultipartRequest) + 8) {
    log::info(
        "MultipartRequest v1 OFPMP_PORT_STATS/OFPMP_QUEUE is wrong length.",
        buf_.size());
    header()->setType(OFPT_UNSUPPORTED);
    return;
  }

  UInt8 *ptr = buf_.mutableData() + sizeof(MultipartRequest);

  // Change port number from 16-bits to 32-bits.
  Big32 *port = Big32_cast(ptr);
  *port = normPortNumberV1(ptr);
}

void Transmogrify::normalizeMPPortOrQueueStatsReplyV3(size_t *start,
                                                      size_t len) {
  // Normalize the PortStatsReply V3 to look like a V4+ message.
  size_t offset = *start;
  size_t remaining = buf_.size() - offset;

  if (remaining < len) {
    *start = buf_.size();
    return;
  }

  UInt8 *ptr = buf_.mutableData() + offset;

  // Insert an additional 8-bytes for timestamp.
  buf_.insertZeros(ptr + len, 8);
  *start += len + 8;
}

void Transmogrify::normalizeMPPortDescReplyV1() {
  using deprecated::PortV1;

  // Verify size of port list.
  size_t portListSize = buf_.size() - sizeof(MultipartReply);
  if ((portListSize % sizeof(PortV1)) != 0) {
    log::debug("Invalid PortDesc port list size.");
    return;
  }

  // Normalize the port structures from V1 to normal size.
  size_t portCount = portListSize / sizeof(PortV1);
  UInt8 *pkt = buf_.mutableData();
  const PortV1 *portV1 =
      reinterpret_cast<const PortV1 *>(pkt + sizeof(MultipartReply));

  PortList ports;
  for (size_t i = 0; i < portCount; ++i) {
    PortBuilder newPort{*portV1};
    ports.add(newPort);
    ++portV1;
  }

  // Copy new port list into packet.
  buf_.addUninitialized(ports.size() - portListSize);
  pkt = buf_.mutableData();
  assert(buf_.size() == sizeof(MultipartReply) + ports.size());
  std::memcpy(pkt + sizeof(MultipartReply), ports.data(), ports.size());

  // Update header length. N.B. Make sure we use current header ptr.
  header()->setLength(UInt16_narrow_cast(buf_.size()));
}

void Transmogrify::normalizeMPPortDescReplyV4() {
  using deprecated::PortV2;

  // Verify size of port list.
  size_t portListSize = buf_.size() - sizeof(MultipartReply);
  if ((portListSize % sizeof(PortV2)) != 0) {
    log::debug("Invalid PortDesc port list size.");
    return;
  }

  // Normalize the port structures from V2 to normal size.
  size_t portCount = portListSize / sizeof(PortV2);
  UInt8 *pkt = buf_.mutableData();
  const PortV2 *portV2 =
      reinterpret_cast<const PortV2 *>(pkt + sizeof(MultipartReply));

  PortList ports;
  for (size_t i = 0; i < portCount; ++i) {
    PortBuilder newPort{*portV2};
    ports.add(newPort);
    ++portV2;
  }

  // Copy new port list into packet.
  buf_.addUninitialized(ports.size() - portListSize);
  pkt = buf_.mutableData();
  assert(buf_.size() == sizeof(MultipartReply) + ports.size());
  std::memcpy(pkt + sizeof(MultipartReply), ports.data(), ports.size());

  // Update header length. N.B. Make sure we use current header ptr.
  header()->setLength(UInt16_narrow_cast(buf_.size()));
}

void Transmogrify::normalizeMPPortStatsReplyV1(size_t *start) {
  // Convert V1 port stats reply into V5 format.

  size_t offset = *start;
  size_t remaining = buf_.size() - offset;

  if (remaining < 104) {
    *start = buf_.size();
    return;
  }

  UInt8 *ptr = buf_.mutableData() + offset;

  // Change port number from 16-bits to 32-bits.
  *Big32_cast(ptr) = normPortNumberV1(ptr);

  // Insert an additional 8-bytes for timestamps.
  buf_.insertZeros(ptr + 104, 8);

  // FIXME(bfish): Reusing the V4 code implies two buffer insertions here.
  normalizeMPPortStatsReplyV4(start);
}

void Transmogrify::normalizeMPPortStatsReplyV2(size_t *start) {
  // Convert V2 port stats reply into V5 format.

  size_t offset = *start;
  size_t remaining = buf_.size() - offset;

  if (remaining < 104) {
    *start = buf_.size();
    return;
  }

  UInt8 *ptr = buf_.mutableData() + offset;

  // Insert an additional 8-bytes for timestamps.
  buf_.insertZeros(ptr + 104, 8);

  // FIXME(bfish): Reusing the V4 code implies two buffer insertions here.
  normalizeMPPortStatsReplyV4(start);
}

void Transmogrify::normalizeMPPortStatsReplyV4(size_t *start) {
  // Convert V4 port stats reply into V5 format.

  size_t offset = *start;
  size_t remaining = buf_.size() - offset;

  if (remaining < 112) {
    *start = buf_.size();
    return;
  }

  // Swap portNo and pad.
  UInt8 *ptr = buf_.mutableData() + offset;
  assert(IsPtrAligned(ptr, 8));
  buf_.insertUninitialized(ptr + 112, 8);
  ptr = buf_.mutableData() + offset;

  UInt32 *portNo = reinterpret_cast<UInt32 *>(ptr);
  UInt32 *pad = reinterpret_cast<UInt32 *>(ptr + sizeof(UInt32));
  std::swap(*portNo, *pad);

  // Copy Ethernet property values.
  PortStatsPropertyEthernet eth;
  std::memcpy(MutableBytePtr(&eth) + 8, ptr + 72, sizeof(UInt64) * 4);

  // Shift down rx_packets et al.
  std::memmove(ptr + 16, ptr + 8, sizeof(UInt64) * 8);

  // Move duration fields up under port No.
  std::memcpy(ptr + 8, ptr + 104, sizeof(UInt32) * 2);

  // Copy Ethernet property into end.
  std::memcpy(ptr + 80, &eth, sizeof(eth));

  // Set length_ field.
  *Big16_cast(ptr) = 120;
  *start += 120;
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

  log::debug("normActionsV1orV2", actions.toByteRange());

  // Return change in length of action list.
  int lengthChange = 0;

  ActionIterator iter = actions.begin();
  ActionIterator iterEnd = actions.end();

  while (iter < iterEnd) {
    ActionType actType = iter->type();
    UInt16 type = actType.enumType();

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
      if (ipProto == PROTOCOL_TCP) {
        lengthChange += normSetField<OFB_TCP_SRC>(iter, iterEnd);
      } else if (ipProto == PROTOCOL_UDP) {
        lengthChange += normSetField<OFB_UDP_SRC>(iter, iterEnd);
      } else if (ipProto == PROTOCOL_ICMP) {
        lengthChange += normSetField<OFB_ICMPV4_TYPE>(iter, iterEnd);
      } else {
        log::info("OFPAT_SET_TP_DST: Unknown proto", ipProto);
      }
      break;
    case v2::OFPAT_SET_TP_DST:
      if (ipProto == PROTOCOL_TCP) {
        lengthChange += normSetField<OFB_TCP_DST>(iter, iterEnd);
      } else if (ipProto == PROTOCOL_UDP) {
        lengthChange += normSetField<OFB_UDP_DST>(iter, iterEnd);
      } else if (ipProto == PROTOCOL_ICMP) {
        lengthChange += normSetField<OFB_ICMPV4_CODE>(iter, iterEnd);
      } else {
        log::info("OFPAT_SET_TP_DST: Unknown proto", ipProto);
      }
      break;
  }

  return lengthChange;
}

int Transmogrify::normOutput(ActionIterator *iter, ActionIterator *iterEnd) {
  if ((*iter)->type() != deprecated::AT_OUTPUT_V1::type())
    return 0;

  ByteRange valueRange = (*iter)->value();

  // Change port number from 16 to 32 bits.
  UInt32 port = normPortNumberV1(valueRange.data());
  UInt16 maxlen = *Big16_cast(valueRange.data() + 2);

  AT_OUTPUT output{port, maxlen};

  ptrdiff_t offset = buf_.offset(iter->data());
  ptrdiff_t endOffset = buf_.offset(iterEnd->data());

  buf_.insertUninitialized(valueRange.data(), 8);

  ActionRange range{{buf_.data() + offset, buf_.data() + endOffset + 8}};
  *iter = range.begin();
  *iterEnd = range.end();

  std::memcpy(RemoveConst_cast(iter->data()), &output, sizeof(output));

  assert((*iter)->type() == AT_OUTPUT::type());

  return 8;
}
