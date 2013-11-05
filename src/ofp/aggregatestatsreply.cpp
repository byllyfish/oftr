#include "ofp/aggregatestatsreply.h"

using namespace ofp;

bool AggregateStatsReply::validateLength(size_t length) const {
  return (length == sizeof(AggregateStatsReply));
}
