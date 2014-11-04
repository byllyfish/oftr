// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/validation.h"
#include "ofp/log.h"
#include "ofp/message.h"

using namespace ofp;

Validation::Validation(const Message *msg) : msg_{msg} {
  if (msg) {
    version_ = msg->version();
    length_ = msg->size();
  } else {
    version_ = 0;
    length_ = 0;
  }
}

void Validation::messageSizeIsInvalid() {
  log::info("Validation Failed: Message size is invalid", length_);
  logContext();
}

void Validation::messageTypeIsNotSupported() {
  log::info("Validation Failed: Message type is not supported");
  logContext();
}

void Validation::multipartTypeIsNotSupported() {
  log::info("Validation Failed: Multipart type is not supported");
  logContext();
}

void Validation::multipartTypeIsNotSupportedForVersion() {
  log::info(
      "Validation Failed: Multipart type is not supported for this protocol "
      "version");
  logContext();
}

void Validation::multipartSizeHasImproperAlignment() {
  log::info("Validation Failed: Multipart size has improper alignment");
  logContext();
}

void Validation::lengthRemainingIsInvalid(const UInt8 *ptr,
                                          size_t expectedLength) {
  log::info("Validation Failed: Length remaining is invalid", lengthRemaining(),
            "expected", expectedLength, "at offset", offset(ptr));
  logContext(ptr);
}

void Validation::rangeSizeHasImproperAlignment(const UInt8 *ptr,
                                               size_t alignment) {
  log::info("Validation Failed: Range length has improper alignment at offset",
            offset(ptr), "alignment", alignment);
  logContext(ptr);
}

void Validation::rangeDataHasImproperAlignment(const UInt8 *ptr,
                                               size_t alignment) {
  log::info("Validation Failed: Range data has improper alignment at offset",
            offset(ptr), "alignment", alignment);
  logContext(ptr);
}

void Validation::rangeElementSizeIsTooSmall(const UInt8 *ptr, size_t minSize) {
  log::info("Validation Failed: Range element size is too small at offset",
            offset(ptr));
  logContext(ptr);
}

void Validation::rangeElementSizeHasImproperAlignment(const UInt8 *ptr,
                                                      size_t elemSize,
                                                      size_t alignment) {
  log::info(
      "Validation Failed: Range element size has improper alignment at offset",
      offset(ptr), "elemSize", elemSize, "alignment", alignment);
  logContext(ptr);
}

void Validation::rangeElementSizeOverrunsEnd(const UInt8 *ptr,
                                             size_t jumpSize) {
  log::info(
      "Validation Failed: Range element size overruns end of buffer at offset",
      offset(ptr), "jumpSize", jumpSize);
  logContext(ptr);
}

void Validation::rangeSizeIsNotMultipleOfElementSize(const UInt8 *ptr,
                                                     size_t elementSize) {
  log::info(
      "Validation Failed: Range size not a multiple of element size at offset",
      offset(ptr), "elementSize", elementSize);
  logContext(ptr);
}

size_t Validation::offset(const UInt8 *ptr) const {
  if (!msg_ || ptr < msg_->data()) {
    return 0xFFFFFFFF;
  }
  return Unsigned_cast(ptr - msg_->data());
}

std::string Validation::hexContext(const UInt8 *ptr) const {
  assert(msg_);

  const UInt8 *data = msg_->data();
  if ((ptr < data) || (ptr >= data + length_)) {
    return "<out of range>";
  }

  size_t len = 16;
  if (Unsigned_cast((data + length_) - ptr) < len) {
    len = Unsigned_cast((data + length_) - ptr);
  }

  return RawDataToHex(ptr, len);
}

void Validation::logContext(const UInt8 *ptr) const {
  assert(msg_ != nullptr || length_ == 0);

  if (length_ >= sizeof(Header)) {
    const Header *header = msg_->header();
    OFPType type = header->type();
    UInt8 version = header->version();

    std::ostringstream oss;
    if (length_ >= 16 &&
        (type == OFPT_MULTIPART_REQUEST || type == OFPT_MULTIPART_REPLY)) {
      OFPMultipartType mpType =
          *reinterpret_cast<const Big<OFPMultipartType> *>(msg_->data() +
                                                           sizeof(Header));
      oss << type << '.' << mpType << 'v' << static_cast<int>(version);
    } else {
      oss << type << 'v' << static_cast<int>(version);
    }

    log::info("  Message type:", oss.str());
    if (ptr) {
      log::info("  Context(16 bytes):", hexContext(ptr));
    }
  }
}
