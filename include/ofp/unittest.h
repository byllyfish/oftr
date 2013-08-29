#ifndef OFP_UNITTEST_H
#define OFP_UNITTEST_H

#include <gtest/gtest.h>
#include "ofp/types.h"
#include "ofp/writable.h"
#include "ofp/bytelist.h"
#include "ofp/padding.h"
#include <cstdio>

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wglobal-constructors"
#endif

inline std::string hexclean(const char *data)
{
    std::string s = ofp::HexToRawData(data);
    return ofp::RawDataToHex(s.data(), s.size());
}

#define EXPECT_HEX(hexstr, data, length)                                       \
    {                                                                          \
        auto hex = hexclean(hexstr);                                           \
        EXPECT_EQ(hex, ofp::RawDataToHex(data, length));                       \
    }

namespace ofp { // <namespace ofp>

#if 0
namespace spec { // <namespace spec>

// Define operator == for structs for use in EXPECT_EQ. Due to C++ lookup rules,
// operator == must be defined in ofp::spec.

template <class T>
inline
bool operator==(T a, T b)
{
    return std::memcmp(&a, &b, sizeof(b)) == 0;
}

} // </namespace spec>
#endif

class MockChannel : public Writable {
public:
    MockChannel(UInt8 version) : version_{version}
    {
    }

    const UInt8 *data() const
    {
        return buf_.data();
    }

    size_t size() const
    {
        return buf_.size();
    }

    UInt8 version() const override
    {
        return version_;
    }

    void write(const void *data, size_t length) override
    {
        buf_.add(data, length);
    }

    void flush() override
    {
    }

    UInt32 nextXid() override;

    template <class MesgBuilderType>
    static ByteList serialize(MesgBuilderType &msg, UInt8 version)
    {
        MockChannel writer{version};
        msg.send(&writer);
        return writer.buf_;
    }

private:
    ByteList buf_;
    UInt32 nextXid_ = 0;
    UInt8 version_;
    Padding<3> pad_;
};

} // </namespace ofp>

#endif // OFP_UNITTEST_H
