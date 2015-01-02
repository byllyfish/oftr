#include "ofp/timestamp.h"
#include <sstream>
#include <iomanip>
#include "ofp/log.h"

using namespace ofp;


static UInt32 pow10[10] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };

bool Timestamp::parse(const std::string &s) {
    UInt64 wholeNum = 0;
    UInt32 fracNum = 0;
    const char *p = s.c_str();

    const char *wholeStart = p;
    while (isdigit(*p)) {
        wholeNum = (wholeNum * 10) + Unsigned_cast(*p - '0');
        ++p;
    }

    if (p == wholeStart)
        return false;

    if (*p != '.')
        return false;
    ++p;

    const char *fracStart = p;
    while (isdigit(*p)) {
        fracNum = (fracNum * 10) + Unsigned_cast(*p - '0');
        ++p;
    }

    if (*p != 0)
        return false;

    auto fracDigits = p - fracStart;
    if (fracDigits == 0 || fracDigits > 9)
        return false;

    fracNum *= pow10[9 - fracDigits];

    seconds_ = static_cast<time_t>(wholeNum);
    nanos_ = fracNum;

    assert(nanos_ < 1000000000);

    return true;
}


std::string Timestamp::toString() const {
    std::stringstream strm;
    strm << seconds_ << '.' << std::setfill('0') << std::setw(9) << nanos_;
    return strm.str();
}
