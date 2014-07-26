#ifndef OFP_VALIDATION_H_
#define OFP_VALIDATION_H_

#include "ofp/byterange.h"
#include "ofp/constants.h"

namespace ofp {

class Validation {
public:
    explicit Validation(const UInt8 *data, size_t length) : data_{data}, length_{length} {}
    explicit Validation(const ByteRange &range) : Validation{range.data(), range.size()} {}

    const UInt8 *data() const { return data_; }
    size_t length() const { return length_; }

    //OFPType type() const;
    //OFPMultipartType multipartType() const;

    size_t lengthRemaining() const { return lengthRemaining_; }
    void setLengthRemaining(size_t length) { lengthRemaining_ = length; }

    void rangeSizeHasImproperAlignment(const UInt8 *ptr, size_t alignment);
    void rangeDataHasImproperAlignment(const UInt8 *ptr, size_t alignment);
    void rangeElementSizeIsTooSmall(const UInt8 *ptr, size_t minSize);
    void rangeElementSizeHasImproperAlignment(const UInt8 *ptr, size_t elemSize, size_t alignment);
    void rangeElementSizeOverrunsEnd(const UInt8 *ptr, size_t jumpSize);

    void rangeSizeIsNotMultipleOfElementSize(const UInt8 *ptr, size_t elementSize);

   // void logErrorInMsg();
   // void logErrorInRange(const void *ptr, ByteRange range, const char *error);

private:
    const UInt8 *data_;
    const size_t length_;
    size_t lengthRemaining_ = 0;

    size_t offset(const UInt8 *ptr) const;
    std::string hexContext(const UInt8 *ptr) const;
};

}  // namespace ofp

#endif // OFP_VALIDATION_H_
