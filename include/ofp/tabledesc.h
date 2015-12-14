#ifndef OFP_TABLEDESC_H_
#define OFP_TABLEDESC_H_

#include "ofp/tablenumber.h"
#include "ofp/propertylist.h"

namespace ofp {

class Writable;

class TableDesc : private NonCopyable {
 public:
  enum { ProtocolIteratorSizeOffset = 0, ProtocolIteratorAlignment = 8 };
  enum { MPVariableSizeOffset = 0 };

  TableNumber tableId() const { return tableId_; }
  OFPTableConfigFlags config() const { return config_; }

  PropertyRange properties() const;

  bool validateInput(Validation *context) const;

 private:
  Big16 length_ = 8;
  TableNumber tableId_;
  Padding<1> pad_;
  Big<OFPTableConfigFlags> config_;

  friend class TableDescBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(TableDesc) == 8, "Unexpected size.");
static_assert(IsStandardLayout<TableDesc>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<TableDesc>(), "Expected trivially copyable.");

class TableDescBuilder {
 public:
  TableDescBuilder() = default;

  void setTableId(TableNumber tableId) { desc_.tableId_ = tableId; }
  void setConfig(OFPTableConfigFlags config) { desc_.config_ = config; }

  void setProperties(const PropertyList &properties) {
    properties_ = properties;
    updateLen();
  }

  size_t writeSize(Writable *channel);
  void write(Writable *channel);

  // Custom assignment operator because desc_ is non-copyable.
  TableDescBuilder &operator=(const TableDescBuilder &table);

 private:
  TableDesc desc_;
  PropertyList properties_;

  enum { SizeWithoutProperties = sizeof(desc_) };

  void updateLen() {
    desc_.length_ =
        UInt16_narrow_cast(SizeWithoutProperties + properties_.size());
  }

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_TABLEDESC_H_
