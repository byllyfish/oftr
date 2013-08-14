#ifndef OFP_UNITTEST_H
#define OFP_UNITTEST_H

#include <gtest/gtest.h>
#include "ofp/types.h"
#include "ofp/writable.h"
#include "ofp/bytelist.h"
#include "ofp/padding.h"
#include <cstdio>


#if defined(__clang__)
# pragma clang diagnostic ignored "-Wglobal-constructors"
#endif


inline std::string TOHEX(const void *data, size_t len)
{
	return ofp::RawDataToHex(data, len);
}

// FIXME unsafe!!! destroying temporary!
inline const char *HEX(const char *data) { return ofp::HexToRawData(data).data(); }

inline std::string HEXCLEAN(const char *data) { 
	std::string s = ofp::HexToRawData(data);
	return ofp::RawDataToHex(s.data(), s.size());
}

//EXPECT_HEX("ffff", s)
// EXPECT_EQ(HEXSTR("ffff"), HEX(s));

template <class T>
inline 
std::string TOHEX(const T &val) { return TOHEX(&val, sizeof(val)); }

// SHould I keep this?
// or just use
//    cout << HEX(data, length) << '\n';
//    
//    
inline void HEXDUMP(const void *data, size_t length)
{
	std::printf("%s\n", TOHEX(data, length).c_str());
}

template <class T>
void HEXDUMP(const T &val) {
	std::printf("%s\n", TOHEX(val).c_str());
}

template <class T>
inline
void HEXDUMP(const std::string &msg, const T &val) {
	std::printf("%s %s\n", msg.c_str(), TOHEX(val).c_str());
}

#define EXPECT_HEX(hexstr, data, length)  { auto hex = HEXCLEAN(hexstr); EXPECT_EQ(hex, TOHEX(data, length)); }

#if 0
inline void EXPECT_HEX(const char *hexstr, const void *data, size_t length)
{
	auto hex = HEXCLEAN(hexstr);
	EXPECT_EQ(hex, TOHEX(data, length));
}

template <class T>
void EXPECT_HEX(const char *hexstr, const T &value) {
	auto hex = HEXCLEAN(hexstr);
	EXPECT_EQ(hex, TOHEX(value));
}
#endif


namespace ofp { // <namespace ofp>
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

    void close() override
    {
    }

    UInt32 nextXid() override;

private:
    ByteList buf_;
    UInt32 nextXid_ = 0;
    UInt8 version_;
    Padding<3> pad_;
};

} // </namespace ofp>


#endif // OFP_UNITTEST_H
		
