#ifndef OFP_PROPERTYRANGE_H_
#define OFP_PROPERTYRANGE_H_

#include "ofp/protocoliterable.h"

namespace ofp {

namespace detail {

class PropertyIteratorItem : private NonCopyable {
public:
    enum { ProtocolIteratorSizeOffset = 2 };
    
    UInt16 property() const { return property_; }
    UInt16 size() const { return len_; }

    template <class PropertyType>
    const PropertyType &castProperty() const {
        return *reinterpret_cast<const PropertyType *>(this);
    }

private:
    Big16 property_;
    Big16 len_;
};

}  // namespace detail


using PropertyIterator = ProtocolIterator<detail::PropertyIteratorItem>;

class PropertyRange : public ProtocolIterable<detail::PropertyIteratorItem, PropertyIterator> {
using Inherited = ProtocolIterable<detail::PropertyIteratorItem, PropertyIterator>;
public:
    using Inherited::Inherited;

    template <class PropertyType>
    Iterator findProperty() const {
        for (auto iter = begin(); iter < end(); ++iter) {
            if (iter->property() == PropertyType::Property && iter->size() == sizeof(PropertyType)) {
                return iter;
            }
        }
        return end();
    }

    template <class PropertyType>
    typename PropertyType::ValueType value() const {
        auto iter = findProperty<PropertyType>();
        if (iter != end()) {
            return iter->template castProperty<PropertyType>().value();
        }
        return PropertyType::defaultValue();
    }
};

}  // namespace ofp

#endif // OFP_PROPERTYRANGE_H_
