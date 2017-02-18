// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/queuedesc.h"
#include "ofp/writable.h"

using namespace ofp;

PropertyRange QueueDesc::properties() const {
  return SafeByteRange(this, len_, sizeof(QueueDesc));
}

bool QueueDesc::validateInput(Validation *context) const {
  if (!context->validateBool(len_ >= sizeof(QueueDesc), "QueueDesc too short"))
    return false;

  return properties().validateInput(context);
}

void QueueDescBuilder::write(Writable *channel) {
  assert(desc_.len_ == sizeof(desc_) + properties_.size());

  channel->write(&desc_, sizeof(desc_));
  channel->write(properties_.data(), properties_.size());

  // FIXME(bfish): Do we want this here?
  channel->flush();
}
