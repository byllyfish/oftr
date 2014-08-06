#include "ofp/validation.h"
#include "ofp/log.h"
#include "ofp/message.h"

using namespace ofp;

Validation::Validation(const Message *msg) : Validation{msg->data(), msg->size()} 
{
}

void Validation::messageSizeIsInvalid() {
    log::info("Validation Failed: Message size is invalid", length_);
    logContext(data_);
}

void Validation::messageTypeIsNotSupported() {
    log::info("Validation Failed: Message type is not supported");
    logContext(data_);
}

void Validation::multipartTypeIsNotSupported() {
    log::info("Validation Failed: Multipart type is not supported");
    logContext(data_);
}

void Validation::lengthRemainingIsInvalid(const UInt8 *ptr, size_t expectedLength) {
    log::info("Validation Failed: Length remaining is invalid", lengthRemaining(), "expected", expectedLength, "at offset", offset(ptr));
    logContext(ptr);
}

void Validation::rangeSizeHasImproperAlignment(const UInt8 *ptr, size_t alignment) {
    log::info("Validation Failed: Range length has improper alignment at offset", offset(ptr), "alignment", alignment);
    logContext(ptr);
}

void Validation::rangeDataHasImproperAlignment(const UInt8 *ptr, size_t alignment) {
    log::info("Validation Failed: Range data has improper alignment at offset", offset(ptr), "alignment", alignment);
    logContext(ptr);
}

void Validation::rangeElementSizeIsTooSmall(const UInt8 *ptr, size_t minSize) {
    log::info("Validation Failed: Range element size is too small at offset", offset(ptr));
    logContext(ptr);
}

void Validation::rangeElementSizeHasImproperAlignment(const UInt8 *ptr, size_t elemSize, size_t alignment) {
    log::info("Validation Failed: Range element size has improper alignment at offset", offset(ptr), "elemSize", elemSize, "alignment", alignment);
    logContext(ptr);
}

void Validation::rangeElementSizeOverrunsEnd(const UInt8 *ptr, size_t jumpSize) {
    log::info("Validation Failed: Range element size overruns end of buffer at offset", offset(ptr), "jumpSize", jumpSize);
    logContext(ptr);
}

void Validation::rangeSizeIsNotMultipleOfElementSize(const UInt8 *ptr, size_t elementSize) {
    log::info("Validation Failed: Range size not a multiple of element size at offset", offset(ptr), "elementSize", elementSize);
    logContext(ptr);
}


size_t Validation::offset(const UInt8 *ptr) const {
    if (ptr < data_) {
        return 0xFFFFFFFF;
    }
    return Unsigned_cast(ptr - data_);
}

std::string Validation::hexContext(const UInt8 *ptr) const {
    if ((ptr < data_) || (ptr >= data_ + length_)) {
        return "<out of range>";
    }

    size_t len = 16;
    if (Unsigned_cast((data_ + length_) - ptr) < len) {
        len = Unsigned_cast((data_ + length_) - ptr);
    }

    return RawDataToHex(ptr, len);
}

void Validation::logContext(const UInt8 *ptr) const {
    if (length_ >= sizeof(Header)) {
        const Header *header = reinterpret_cast<const Header *>(data_);
        OFPType type = header->type();
        UInt8 version = header->version();

        std::ostringstream oss;
        if (length_ >= 16 && (type == OFPT_MULTIPART_REQUEST || type == OFPT_MULTIPART_REPLY)) {
            OFPMultipartType mpType = *reinterpret_cast<const Big<OFPMultipartType> *>(data_ + sizeof(Header));
            oss << type << '.' << mpType << " v1." << static_cast<int>(version-1);
        } else {
            oss << type << " v1." << static_cast<int>(version-1);
        }

        log::info("  Message type:", oss.str());
        log::info("  Context(16 bytes):", hexContext(ptr));
    }
}
