#include "ofp/types.h"
#include <cassert>


constexpr char HexDigits[17] = "0123456789ABCDEF";


inline
uint8_t FromHex(char hex) {
	assert(std::isxdigit(hex));
	return hex <= '9' ? hex - '0' : (hex & ~0x20) - 'A' + 10;
}

inline
char ToHex(uint8_t value) {
	assert(value < 16);
	return HexDigits[value];
}


std::string ofp::sys::RawDataToHex(const void *data, size_t len)
{
	std::string result;
	result.reserve(2*len);

	const uint8_t *pos = reinterpret_cast<const uint8_t*>(data);
	const uint8_t *end = pos + len;
	
	char buf[2];
	while (pos < end) {
		buf[0] = ToHex(*pos >> 4);
		buf[1] = ToHex(*pos++ & 0x0F);
		result.append(buf, sizeof(buf));
	}
	
	return result;	
}

size_t ofp::sys::HexToRawData(const std::string &hex, void *data, size_t maxlen)
{
	uint8_t *begin = reinterpret_cast<uint8_t*>(data);
	uint8_t *end = begin + maxlen;
	
	uint8_t ch[2];
	unsigned idx = 0;
	uint8_t *out = begin;
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
