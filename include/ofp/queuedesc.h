#ifndef OFP_QUEUEDESC_H_
#define OFP_QUEUEDESC_H_

#include "ofp/portnumber.h"
#include "ofp/queuenumber.h"
#include "ofp/propertylist.h"

namespace ofp {

class Writable;

class QueueDesc : private NonCopyable {
public:
  // Offset of len_ field.
  enum { ProtocolIteratorSizeOffset = 8, ProtocolIteratorAlignment = 8 };
  enum { MPVariableSizeOffset = 8 };

  QueueNumber queueId() const { return queueId_; }
  PortNumber port() const { return portNo_; }

  PropertyRange properties() const;

  bool validateInput(Validation *context) const;

private:
  PortNumber portNo_;
  QueueNumber queueId_;
  Big16 len_;
  Padding<6> pad_;

  // Only a QueueDescBuilder can create an instance.
  QueueDesc() = default;

  friend class QueueDescBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(QueueDesc) == 16, "Unexpected size.");
static_assert(IsStandardLayout<QueueDesc>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<QueueDesc>(), "Expected trivially copyable.");

class QueueDescBuilder {
public:
    QueueDescBuilder() = default;

    void setQueueId(QueueNumber queueId) { desc_.queueId_ = queueId; }
    void setPort(PortNumber portNo) { desc_.portNo_ = portNo; }

    void setProperties(const PropertyList &properties) { properties_ = properties; updateLen(); }

    size_t writeSize(Writable *channel);
    void write(Writable *channel);
    void reset() {}

private:
    QueueDesc desc_;
    PropertyList properties_;

    void updateLen() { desc_.len_ = UInt16_narrow_cast(sizeof(desc_) + properties_.size()); }

    template <class T>
    friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif // OFP_QUEUEDESC_H_
