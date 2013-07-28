#include "ofp/types.h"
#include <cassert>


constexpr char HexDigits[17] = "0123456789ABCDEF";


inline
ofp::UInt8 FromHex(char hex) {
	assert(std::isxdigit(hex));
	return hex <= '9' ? hex - '0' : (hex & ~0x20) - 'A' + 10;
}

inline
char ToHex(ofp::UInt8 value) {
	assert(value < 16);
	return HexDigits[value];
}


std::string ofp::RawDataToHex(const void *data, size_t len)
{
	std::string result;
	result.reserve(2*len);

	const UInt8 *pos = reinterpret_cast<const UInt8*>(data);
	const UInt8 *end = pos + len;
	
	char buf[2];
	while (pos < end) {
		buf[0] = ToHex(*pos >> 4);
		buf[1] = ToHex(*pos++ & 0x0F);
		result.append(buf, sizeof(buf));
	}
	
	return result;	
}

size_t ofp::HexToRawData(const std::string &hex, void *data, size_t maxlen)
{
	UInt8 *begin = reinterpret_cast<UInt8*>(data);
	UInt8 *end = begin + maxlen;
	
	UInt8 ch[2];
	unsigned idx = 0;
	UInt8 *out = begin;
	for (auto inp = hex.c_str(); *inp; ++inp) {
		if (std::isxdigit(*inp)) {
			ch[idx++] = *inp;
			if (idx >= 2) {
				*out++ = (FromHex(ch[0]) << 4) | FromHex(ch[1]);
				if (out >= end)
					break;
				idx = 0;
			}
		}
	}
	
	std::memset(out, 0, end - out);
	
	return out - begin;
}
