#include "ofp/queuegetconfigreply.h"

using ofp::QueueGetConfigReply;

bool QueueGetConfigReply::validateInput(size_t length) const {
  // TODO(bfish) implement correctly...
  return length == sizeof(QueueGetConfigReply);
}
