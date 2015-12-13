#include "ofp/tabledesc.h"
#include "ofp/writable.h"

using namespace ofp;

PropertyRange TableDesc::properties() const {
  return SafeByteRange(this, length_, sizeof(TableDesc));
}

bool TableDesc::validateInput(Validation *context) const {
  return properties().validateInput(context);
}

size_t TableDescBuilder::writeSize(Writable *channel) {
  return sizeof(desc_) + properties_.size();
}

void TableDescBuilder::write(Writable *channel) {
  assert(desc_.length_ == SizeWithoutProperties + properties_.size());

  channel->write(&desc_, sizeof(desc_));
  channel->write(properties_.data(), properties_.size());
}

TableDescBuilder &TableDescBuilder::operator=(const TableDescBuilder &table) {
  if (&table != this) {
    std::memcpy(&desc_, &table.desc_, sizeof(desc_));
    properties_ = table.properties_;
  }
  return *this;
}
