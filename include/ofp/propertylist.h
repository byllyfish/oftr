#ifndef OFP_PROPERTYLIST_H_
#define OFP_PROPERTYLIST_H_

#include "ofp/protocollist.h"
#include "ofp/propertyrange.h"
#include "ofp/queueproperty.h"

namespace ofp {

class PropertyList : public ProtocolList<PropertyRange> {
 public:
  template <class PropertyType>
  void add(const PropertyType &property) {
    buf_.add(&property, sizeof(property));
  }
};

template <>
inline void PropertyList::add(const QueuePropertyExperimenter &property) {
  ByteRange value = property.value();
  buf_.add(&property, QueuePropertyExperimenter::FixedHeaderSize);
  buf_.add(value.data(), value.size());
  buf_.addZeros(PadLength(value.size()) - value.size());
}

}  // namespace ofp

#endif  // OFP_PROPERTYLIST_H_
