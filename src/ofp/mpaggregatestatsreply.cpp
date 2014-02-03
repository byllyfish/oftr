#include "ofp/mpaggregatestatsreply.h"

using namespace ofp;

bool MPAggregateStatsReply::validateLength(size_t length) const {
  return (length == sizeof(MPAggregateStatsReply));
}
