#ifndef OFP_TIMESTAMP_H_
#define OFP_TIMESTAMP_H_

#include "ofp/types.h"

namespace ofp {

OFP_BEGIN_IGNORE_PADDING

class Timestamp {
 public:
  Timestamp() : seconds_{0}, nanos_{0} {}
  Timestamp(time_t seconds, UInt32 nanos) : seconds_{seconds}, nanos_{nanos} {}

  bool parse(const std::string &s);
  bool valid() const { return !(seconds_ == 0 && nanos_ == 0); }

  std::string toString() const;

  bool operator==(const Timestamp &rhs) const {
    return seconds_ == rhs.seconds_ && nanos_ == rhs.nanos_;
  }

 private:
  time_t seconds_;
  UInt32 nanos_;
};

OFP_END_IGNORE_PADDING

}  // namespace ofp

#endif  // OFP_TIMESTAMP_H_
