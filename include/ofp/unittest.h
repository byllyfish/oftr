#ifndef OFP_UNITTEST_H
#define OFP_UNITTEST_H
		
#include <gtest/gtest.h>
#include "ofp/types.h"
#include <cstdio>

inline std::string TOHEX(const void *data, size_t len)
{
	return ofp::RawDataToHex(data, len);
}

// FIXME unsafe!!! destroying temporary!
inline const char *HEX(const char *data) { return ofp::HexToRawData(data).data(); }

inline std::string HEXSTR(const char *data) { 
	std::string s = ofp::HexToRawData(data);
	return ofp::RawDataToHex(s.data(), s.size());
}

//EXPECT_HEX("ffff", s)
// EXPECT_EQ(HEXSTR("ffff"), HEX(s));

template <class T>
inline 
std::string TOHEX(const T &val) { return TOHEX(&val, sizeof(val)); }

template <class T>
void HEXDUMP(const T &val) {
	std::printf("%s\n", TOHEX(val).c_str());
}

template <class T>
inline
void HEXDUMP(const std::string &msg, const T &val) {
	std::printf("%s %s\n", msg.c_str(), TOHEX(val).c_str());
}


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
} // </namespace ofp>


#endif // OFP_UNITTEST_H
		
