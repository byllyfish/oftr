// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_TABLEMOD_H_
#define OFP_TABLEMOD_H_

#include "ofp/protocolmsg.h"
#include "ofp/padding.h"
#include "ofp/tablenumber.h"

namespace ofp {

class TableMod : public ProtocolMsg<TableMod, OFPT_TABLE_MOD, 16, 16> {
 public:
  TableNumber tableId() const { return tableId_; }
  UInt32 config() const { return config_; }

  bool validateInput(Validation *context) const { return true; }

 private:
  Header header_;
  TableNumber tableId_;
  Padding<3> pad_;
  Big32 config_;

  // Only TableModBuilder can create an instance.
  TableMod() : header_{type()} {}

  friend class TableModBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(TableMod) == 16, "Unexpected size.");
static_assert(IsStandardLayout<TableMod>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<TableMod>(), "Expected trivially copyable.");

class TableModBuilder {
 public:
  TableModBuilder() = default;
  explicit TableModBuilder(const TableMod *msg);

  void setTableId(TableNumber tableId) { msg_.tableId_ = tableId; }
  void setConfig(UInt32 config) { msg_.config_ = config; }

  UInt32 send(Writable *channel);

 private:
  TableMod msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_TABLEMOD_H_
