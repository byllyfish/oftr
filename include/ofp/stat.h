#ifndef OFP_STAT_H_
#define OFP_STAT_H_

#include "ofp/statheader.h"
#include "ofp/oxmlist.h"

namespace ofp {

class Stat {
 public:
  Stat() = default;
  explicit Stat(const StatHeader *statHeader) { oxm_ = statHeader->oxmRange(); }

  OXMIterator begin() const { return oxm_.begin(); }
  OXMIterator end() const { return oxm_.end(); }

  OXMRange toRange() const { return oxm_.toRange(); }

 private:
  OXMList oxm_;
};

}  // namespace ofp

#endif // OFP_STAT_H_
