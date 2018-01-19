// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/tabledesc.h"
#include "ofp/writable.h"

using namespace ofp;

PropertyRange TableDesc::properties() const {
  return SafeByteRange(this, length_, sizeof(TableDesc));
}

bool TableDesc::validateInput(Validation *context) const {
  size_t remaining = context->lengthRemaining();
  if (remaining < sizeof(TableDesc))
    return false;

  size_t len = length_;
  if (len < sizeof(TableDesc) || len > remaining)
    return false;

  return properties().validateInput(context);
}

size_t TableDescBuilder::writeSize(Writable *channel) {
  return sizeof(desc_) + properties_.size();
}

void TableDescBuilder::write(Writable *channel) {
  assert(desc_.length_ == sizeof(desc_) + properties_.size());

  channel->write(&desc_, sizeof(desc_));
  channel->write(properties_.data(), properties_.size());

  // FIXME(bfish): Do we want this here?
  channel->flush();
}

TableDescBuilder &TableDescBuilder::operator=(const TableDescBuilder &table) {
  if (&table != this) {
    std::memcpy(&desc_, &table.desc_, sizeof(desc_));
    properties_ = table.properties_;
  }
  return *this;
}
