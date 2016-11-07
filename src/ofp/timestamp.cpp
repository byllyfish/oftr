// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/timestamp.h"
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace ofp;

const UInt32 kNanosecondsPerSec = 1000000000;

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

  UInt64 diff = Unsigned_cast(seconds() - ts.seconds());
  if (nanoseconds() >= ts.nanoseconds()) {
    return diff +
           static_cast<double>(nanoseconds() - ts.nanoseconds()) /
               kNanosecondsPerSec;
  }

  assert(diff > 0);
  return (diff - 1) +
         static_cast<double>(kNanosecondsPerSec - ts.nanoseconds() +
                             nanoseconds()) /
             kNanosecondsPerSec;
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
  assert(fracNum < 1000000000);

  time_.first = static_cast<time_t>(wholeNum);
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

  auto secs = seconds();
  auto nsec = nanoseconds();

  struct tm date;
  gmtime_r(&secs, &date);
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
  auto nano = duration_cast<std::chrono::nanoseconds>(duration).count();

  return Timestamp{static_cast<time_t>(nano / 1000000000),
                   UInt32_narrow_cast(nano % 1000000000)};
}
