#include "ofp/validation.h"
#include "ofp/log.h"

using namespace ofp;

void Validation::rangeSizeHasImproperAlignment(const UInt8 *ptr, size_t alignment) {
    log::info("Validation Failed: Range length has improper alignment at offset", offset(ptr), "alignment", alignment);
    log::info("  Context(16 bytes):", hexContext(ptr));
}

void Validation::rangeDataHasImproperAlignment(const UInt8 *ptr, size_t alignment) {
    log::info("Validation Failed: Range data has improper alignment at offset", offset(ptr), "alignment", alignment);
    log::info("  Context(16 bytes):", hexContext(ptr));
}

void Validation::rangeElementSizeIsTooSmall(const UInt8 *ptr, size_t minSize) {
    log::info("Validation Failed: Range element size is too small at offset", offset(ptr));
    log::info("  Context(16 bytes):", hexContext(ptr));
}

void Validation::rangeElementSizeHasImproperAlignment(const UInt8 *ptr, size_t elemSize, size_t alignment) {
    log::info("Validation Failed: Range element size has improper alignment at offset", offset(ptr), "elemSize", elemSize, "alignment", alignment);
    log::info("  Context(16 bytes):", hexContext(ptr));
}

void Validation::rangeElementSizeOverrunsEnd(const UInt8 *ptr, size_t jumpSize) {
    log::info("Validation Failed: Range element size overruns end of buffer at offset", offset(ptr), "jumpSize", jumpSize);
    log::info("  Context(16 bytes):", hexContext(ptr));
}

void Validation::rangeSizeIsNotMultipleOfElementSize(const UInt8 *ptr, size_t elementSize) {
    log::info("Validation Failed: Range size not a multiple of element size at offset", offset(ptr), "elementSize", elementSize);
    log::info("  Context(16 bytes):", hexContext(ptr));
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
