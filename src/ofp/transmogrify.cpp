// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/transmogrify.h"
#include "ofp/actions.h"
#include "ofp/experimenter.h"
#include "ofp/featuresreply.h"
#include "ofp/flowmod.h"
#include "ofp/flowremoved.h"
#include "ofp/getasyncreply.h"
#include "ofp/instructionrange.h"
#include "ofp/instructions.h"
#include "ofp/message.h"
#include "ofp/multipartreply.h"
#include "ofp/multipartrequest.h"
#include "ofp/originalmatch.h"
#include "ofp/packetout.h"
#include "ofp/portmod.h"
#include "ofp/portstatsproperty.h"
#include "ofp/portstatus.h"
#include "ofp/queuegetconfigreply.h"
#include "ofp/setasync.h"

using namespace ofp;

using deprecated::OriginalMatch;
using deprecated::StandardMatch;

// This is defined here instead of Transmogrify.h because of header
// dependencies.
Transmogrify::Transmogrify(Message *message) : buf_(message->buf_) {}

void Transmogrify::normalize() {
  if (buf_.size() < sizeof(Header)) {
    log::fatal("Transmogrify::normalize called with invalid message");
  }

  Header *hdr = header();

  // Translate type of message from earlier version into latest enum.
  OFPType type =
      Header::translateType(hdr->version(), hdr->type(), OFP_VERSION_4);
  if (type == OFPT_UNSUPPORTED) {
    markInputInvalid("Unsupported type for protocol version");
  }
  hdr->setType(type);

#if !defined(NDEBUG)
  if ((type == MultipartReply::type() || type == MultipartRequest::type()) &&
      buf_.size() >= 16) {
    UInt16 multiType = *Big16_cast(buf_.data() + 8);
    log_debug("normalize", type, static_cast<OFPMultipartType>(multiType));
  } else {
    log_debug("normalize", type);
  }
#endif  // !defined(NDEBUG)

  // If header length doesn't match buffer size, we have a problem. Under normal
  // conditions, this should never be triggered.
  if (buf_.size() != hdr->length()) {
    markInputInvalid("Unexpected header length");
    hdr->setLength(buf_.size());
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
    } else if (type == SetAsync::type() || type == GetAsyncReply::type()) {
      normalizeAsyncConfigV4();
    } else if (type == QueueGetConfigReply::type()) {
      normalizeQueueGetConfigReplyV2();
    }
  } else if (version >= OFP_VERSION_5) {
    if (type == MultipartReply::type()) {
      normalizeMultipartReplyV5();
    }
  }

  if (buf_.size() > OFP_MAX_SIZE) {
    markInputTooBig("Message truncated to OFP_MAX_SIZE");
    buf_.resize(OFP_MAX_SIZE);
  }

  header()->setLength(buf_.size());
  assert(buf_.size() == header()->length());
}

void Transmogrify::normalizeFeaturesReplyV1() {
  using deprecated::PortV1;

  Header *hdr = header();

  // Check minimum size requirement.
  if (hdr->length() < sizeof(FeaturesReply)) {
    markInputInvalid("FeaturesReply is too short");
    return;
  }

  // Verify size of port list.
  size_t portListSize = hdr->length() - sizeof(FeaturesReply);
  if ((portListSize % sizeof(PortV1)) != 0) {
    markInputInvalid("FeaturesReply has invalid port list size");
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
    markInputTooBig("FeaturesReply has too many ports");
    return;
  }

  // Copy new port list into packet.
  buf_.addUninitialized(ports.size() - portListSize);
  pkt = buf_.mutableData();
  assert(buf_.size() == sizeof(FeaturesReply) + ports.size());
  std::memcpy(pkt + sizeof(FeaturesReply), ports.data(), ports.size());
}

void Transmogrify::normalizeFeaturesReplyV2() {
  using deprecated::PortV2;

  Header *hdr = header();

  if (hdr->length() < sizeof(FeaturesReply)) {
    markInputInvalid("FeaturesReply is too short");
    return;
  }

  // Verify size of port list.
  size_t portListSize = hdr->length() - sizeof(FeaturesReply);
  if ((portListSize % sizeof(PortV2)) != 0) {
    markInputInvalid("FeaturesReply has invalid port list size");
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
    markInputTooBig("FeaturesReply has too many ports");
    return;
  }

  // Copy new port list into packet.
  buf_.addUninitialized(ports.size() - portListSize);
  pkt = buf_.mutableData();
  assert(buf_.size() == sizeof(FeaturesReply) + ports.size());
  std::memcpy(pkt + sizeof(FeaturesReply), ports.data(), ports.size());
}

void Transmogrify::normalizeFlowModV1() {
  Header *hdr = header();

  if (hdr->length() < 72) {
    markInputInvalid("FlowMod is too short");
    return;
  }

  if (buf_.size() > 65535 - 72) {
    markInputTooBig("FlowMod is too big");
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
    markInputInvalid("FlowMod action size is misaligned");
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
      markInputInvalid("FlowMod actions are invalid");
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
}

void Transmogrify::normalizePortStatusV1() {
  using deprecated::PortV1;
  const size_t PortStatusSize = 16;

  Header *hdr = header();

  if (hdr->length() != PortStatusSize + sizeof(PortV1)) {
    markInputInvalid("PortStatus has invalid size");
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
}

void Transmogrify::normalizePortStatusV2() {
  using deprecated::PortV2;
  const size_t PortStatusSize = 16;

  Header *hdr = header();

  if (hdr->length() != PortStatusSize + sizeof(PortV2)) {
    markInputInvalid("PortStatus has invalid size");
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
}

void Transmogrify::normalizeExperimenterV1() {
  Header *hdr = header();

  if (hdr->length() < 12) {
    markInputInvalid("Experimenter is too short");
    return;
  }

  if (hdr->length() > 65535 - 4) {
    markInputTooBig("Experimenter is too big");
    return;
  }

  // Insert four zero bytes at position 12.
  Padding<4> pad;
  buf_.insert(buf_.data() + 12, &pad, sizeof(pad));
}

void Transmogrify::normalizePacketOutV1() {
  Header *hdr = header();

  if (hdr->length() < 16) {
    markInputInvalid("PacketOut is too short");
    return;
  }

  if (buf_.size() > 65535 - 8) {
    markInputTooBig("PacketOut is too big");
    return;
  }

  // 1. Convert inPort from 16 to 32 bits.
  // 2. Insert 6 pad bytes after 32 bit inPort.
  // 3. Normalize actions, if present.

  UInt8 *pkt = buf_.mutableData();
  Big32 inPort = normPortNumberV1(pkt + 12);
  UInt16 actionLen = *Big16_cast(pkt + 14);

  if ((actionLen % 8) != 0) {
    markInputInvalid("PacketOut action size is invalid");
    return;
  } else if (16u + actionLen > buf_.size()) {
    markInputInvalid("PacketOut action size overruns end");
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
      markInputInvalid("PacketOut has invalid actions");
      return;
    }

    int lengthChange = normActionsV1orV2(actions, 0);
    pkt = buf_.mutableData();

    actionLen += lengthChange;
  }

  Big16 bigLen = actionLen;
  std::memcpy(pkt + 16, &bigLen, sizeof(bigLen));
}

void Transmogrify::normalizePortModV1() {
  Header *hdr = header();

  if (hdr->length() < 32) {
    markInputInvalid("PortMod is too short");
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
}

void Transmogrify::normalizePortModV2() {
  Header *hdr = header();

  if (hdr->length() < 40) {
    markInputInvalid("PortMod is too short");
    return;
  }

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
    markInputInvalid("FlowRemoved has invalid size");
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
}

void Transmogrify::normalizeMultipartRequestV1() {
  Header *hdr = header();
  if (hdr->length() < MultipartRequest::UnpaddedSizeVersion1) {
    markInputInvalid("MultipartRequest is too short");
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
}

void Transmogrify::normalizeMultipartReplyV1() {
  Header *hdr = header();
  if (hdr->length() < MultipartReply::UnpaddedSizeVersion1) {
    markInputInvalid("MultipartReply is too short");
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
}

void Transmogrify::normalizeMultipartReplyV2() {
  Header *hdr = header();
  if (hdr->length() < sizeof(MultipartReply)) {
    markInputInvalid("MultipartReply is too short");
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
}

void Transmogrify::normalizeMultipartReplyV3() {
  Header *hdr = header();
  if (hdr->length() < sizeof(MultipartReply)) {
    markInputInvalid("MultipartReply is too short");
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
}

void Transmogrify::normalizeMultipartReplyV4() {
  Header *hdr = header();
  if (hdr->length() < sizeof(MultipartReply)) {
    markInputInvalid("MultipartReply is too short");
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
}

void Transmogrify::normalizeMultipartReplyV5() {
  Header *hdr = header();
  if (hdr->length() < sizeof(MultipartReply)) {
    markInputInvalid("MultipartReply is too short");
    return;
  }

  const MultipartReply *multipartReply =
      reinterpret_cast<const MultipartReply *>(hdr);

  OFPMultipartType replyType = multipartReply->replyType();
  size_t offset = sizeof(MultipartReply);

  if (replyType == OFPMP_TABLE) {
    while (offset < buf_.size())
      normalizeMPTableStatsReplyV4(&offset);
    assert(offset == buf_.size());
  }
}

void Transmogrify::normalizeAsyncConfigV4() {
  // Both GetAsyncReply and SetAsync have the same message layout.
  Header *hdr = header();
  if (hdr->length() != 32) {
    markInputInvalid("AsyncConfig has invalid size");
    return;
  }

  // New size is 56 bytes. Up from 32 bytes.
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
}

void Transmogrify::normalizeQueueGetConfigReplyV1() {
  Header *hdr = header();

  size_t length = hdr->length();
  if (length < sizeof(QueueGetConfigReply)) {
    markInputInvalid("QueueGetConfigReply is too short");
    return;
  }

  if (hdr->version() == OFP_VERSION_1) {
    // Convert port number to 32-bits -- only for OpenFlow 1.0.
    UInt8 *ptr = buf_.mutableData() + sizeof(Header);
    PortNumber port = PortNumber::fromV1(*Big16_cast(ptr));
    std::memcpy(ptr, &port, sizeof(port));
  }

  // Convert QueueV1 to Queue.
  ByteRange data = SafeByteRange(buf_.mutableData(), buf_.size(),
                                 sizeof(QueueGetConfigReply));
  deprecated::QueueV1Range queues{data};

  Validation context;
  if (!queues.validateInput(&context)) {
    markInputInvalid("QueueGetConfigReply has invalid queues");
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
    markInputTooBig("QueueGetConfigReply is too big");
    return;
  }

  buf_.replace(data.begin(), data.end(), newQueues.data(), newQueues.size());
}

void Transmogrify::normalizeQueueGetConfigReplyV2() {
  // Pad all queues to a multiple of 8. You can only get a queue whose size is
  // not a multiple of 8 if it contains an experimenter property with an
  // unusual length.

  Header *hdr = header();

  size_t length = hdr->length();
  if (length < sizeof(QueueGetConfigReply))
    return;

  ByteRange data = SafeByteRange(buf_.mutableData(), buf_.size(),
                                 sizeof(QueueGetConfigReply));

  size_t remaining = data.size();
  const UInt8 *ptr = data.data();

  ByteList newBuf;
  while (remaining > 16) {
    // Read 16-bit queue length from a potentially mis-aligned position.
    UInt16 queueLen = Big16_copy(ptr + 8);
    if (queueLen > remaining || queueLen < 16)
      return;
    // Copy queue header.
    newBuf.add(ptr, 16);

    // Iterate over properties and pad out the properties whose sizes are not
    // multiples of 8.
    const UInt8 *prop = ptr + 16;
    size_t propLeft = queueLen - 16;
    while (propLeft > 4) {
      UInt16 propSize = Big16_copy(prop + 2);
      if (propSize > propLeft || propSize < 4)
        return;
      newBuf.add(prop, propSize);
      if ((propSize % 8) != 0) {
        newBuf.addZeros(PadLength(propSize) - propSize);
      }
      prop += propSize;
      propLeft -= propSize;
    }

    if (propLeft != 0) {
      log_debug("normalizeQueueGetConfigReplyV2: propLeft != 0");
      return;
    }

    ptr += queueLen;
    assert(prop == ptr);

    remaining -= queueLen;
  }

  if (remaining != 0) {
    log_debug("normalizeQueueGetConfigReplyV2: remaining != 0");
    return;
  }

  // When padding the regular `Queue` structure, we may exceed the max
  // message size of 65535.
  if (newBuf.size() > 65535 - sizeof(QueueGetConfigReply)) {
    markInputTooBig("QueueGetConfigReply is too big");
    return;
  }

  buf_.replace(data.begin(), data.end(), newBuf.data(), newBuf.size());
}

void Transmogrify::normalizeMPFlowRequestV1() {
  // Check length of packet.
  if (buf_.size() != sizeof(MultipartRequest) + 44) {
    markInputInvalid("MultipartRequest.Flow has invalid size");
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
    markInputInvalid("MultipartReply.FlowStats size overruns end");
    *start = buf_.size();
    return;
  }

  if (length < 88) {
    markInputInvalid("MultipartReply.FlowStats is too short");
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
    markInputInvalid("MultipartReply.FlowStats action size is invalid");
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
  *Big16_cast(ptr) = UInt16_narrow_cast(length + needed);
  *start = Unsigned_cast(Signed_cast(*start) + length + needed);
}

void Transmogrify::normalizeMPTableStatsReplyV4(size_t *start) {
  // Normalize the TableStatsReply V4 to look like a V1 message.
  size_t offset = *start;
  UInt8 *ptr = buf_.mutableData() + offset;
  size_t length = buf_.size();

  if (length - offset < 24) {
    markInputInvalid("MultipartReply.TableStats is too short");
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
    markInputInvalid("MultipartRequest.PortStats/Queue has invalid size");
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
    markInputInvalid("MultipartReply.PortDesc has invalid size");
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
}

void Transmogrify::normalizeMPPortDescReplyV4() {
  using deprecated::PortV2;

  // Verify size of port list.
  size_t portListSize = buf_.size() - sizeof(MultipartReply);
  if ((portListSize % sizeof(PortV2)) != 0) {
    markInputInvalid("MultipartReply.PortDesc has invalid size");
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
        log_info("OFPAT_SET_TP_DST: Unknown proto", ipProto);
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
        log_info("OFPAT_SET_TP_DST: Unknown proto", ipProto);
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

void Transmogrify::markInputTooBig(const char *msg) {
  log_warning("Normalize:", msg);
  header()->setVersion(Message::kTooBigErrorFlag | header()->version());
}

void Transmogrify::markInputInvalid(const char *msg) {
  log_warning("Normalize:", msg);
  header()->setVersion(Message::kInvalidErrorFlag | header()->version());
}
