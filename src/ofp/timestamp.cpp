// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/timestamp.h"
#include "ofp/log.h"
#include <iomanip>
#include <sstream>
#include <ctime>      // for gmtime_r, gmtime_s
#include "llvm/Support/Format.h"

using namespace ofp;

inline struct tm *my_gmtime_r(const time_t *t, struct tm *result) {
#if defined(_WIN32)
  // N.B. Arguments reversed; returns errno_t.
  if (gmtime_s(result, t) == 0) {
    return result;
  }
  return nullptr;
#else
  return gmtime_r(t, result);
#endif
}

/// \brief Compute difference in seconds.
///
/// \returns elapsed seconds between `this` and `ts.
///
/// Value is negative if ts > this.
double Timestamp::secondsSince(const Timestamp &ts) const {
  if (ts > *this) {
    return -ts.secondsSince(*this);
  }

  assert(*this >= ts);

  UInt64 diff = seconds() - ts.seconds();
  if (nanoseconds() >= ts.nanoseconds()) {
    return diff +
           static_cast<double>(nanoseconds() - ts.nanoseconds()) / NANO_UNITS;
  }

  assert(diff > 0);
  return (diff - 1) +
         static_cast<double>(NANO_UNITS - ts.nanoseconds() + nanoseconds()) /
             NANO_UNITS;
}

static const UInt32 kPower10[10] = {
    1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};

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

  fracNum *= kPower10[9 - fracDigits];
  assert(fracNum < NANO_UNITS);

  time_.first = wholeNum;
  time_.second = fracNum;

  return true;
}

std::string Timestamp::toString() const {
  std::stringstream strm;
  strm << time_.first << '.' << std::setfill('0') << std::setw(9)
       << time_.second;
  return strm.str();
}

std::string Timestamp::toStringUTC() const {
  char buf[TS_BUFSIZE];
  auto len = toStringUTC(buf);
  return std::string{buf, len};
}

size_t Timestamp::toStringUTC(char (&buf)[TS_BUFSIZE]) const {
  const int TIMESTAMP_LEN = 30;
  static_assert(TIMESTAMP_LEN + 1 < sizeof(buf), "Buffer too small");

  time_t secs = unix_time();
  auto nsec = nanoseconds();

  struct tm date;
  if (!my_gmtime_r(&secs, &date)) {
    ofp::log::fatal("Timestamp::gmtime_r failed", secs);
  }

  date.tm_year += 1900;
  date.tm_mon += 1;

  int rc = snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02d.%09dZ",
                    date.tm_year, date.tm_mon, date.tm_mday, date.tm_hour,
                    date.tm_min, date.tm_sec, nsec);
  assert(rc == TIMESTAMP_LEN);
  return (rc == TIMESTAMP_LEN) ? TIMESTAMP_LEN : 0;
}

Timestamp Timestamp::now() {
  using namespace std::chrono;

  auto now = system_clock::now();
  auto duration = now.time_since_epoch();
  UInt64 nano =
      Unsigned_cast(duration_cast<std::chrono::nanoseconds>(duration).count());

  return Timestamp{nano / NANO_UNITS, UInt32_narrow_cast(nano % NANO_UNITS)};
}

void Timestamp::addSeconds(int seconds) {
  if (seconds >= 0) {
    time_.first += Unsigned_cast(seconds);
  } else {
    time_.first -= Unsigned_cast(-seconds);
  }
}

namespace ofp {

llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const Timestamp &value) {
  return os << llvm::format("%llu.%09u", value.time_.first, value.time_.second);
}

}  // namespace ofp
