#include "ofp/byterange.h"
#include "ofp/bytelist.h"

using namespace ofp;

ByteRange::ByteRange(const ByteList &data)
    : begin_{data.begin()}, end_{data.end()}
{
}