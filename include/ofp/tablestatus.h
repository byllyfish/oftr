// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_TABLESTATUS_H_
#define OFP_TABLESTATUS_H_

#include "ofp/protocolmsg.h"
#include "ofp/tabledesc.h"

namespace ofp {

class TableStatus : public ProtocolMsg<TableStatus, OFPT_TABLE_STATUS, 16> {
 public:
  OFPTableStatusReason reason() const { return reason_; }

  const TableDesc &table() const {
    return *Interpret_cast<TableDesc>(BytePtr(this) + 16);
  }

  bool validateInput(Validation *context) const;

 private:
  Header header_;
  Big<OFPTableStatusReason> reason_;
  Padding<7> pad_;

  // Only TableStatusBuilder can create an instance.
  TableStatus() : header_{type()} {}

  friend class TableStatusBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(TableStatus) == 16, "Unexpected size.");
static_assert(IsStandardLayout<TableStatus>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<TableStatus>(),
              "Expected trivially copyable.");

class TableStatusBuilder {
 public:
  TableStatusBuilder() = default;
  explicit TableStatusBuilder(const TableStatus *msg) : msg_{*msg} {}

  void setReason(OFPTableStatusReason reason) { msg_.reason_ = reason; }
  void setTable(const TableDescBuilder &table) { table_ = table; }

  UInt32 send(Writable *channel);

 private:
  TableStatus msg_;
  TableDescBuilder table_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_TABLESTATUS_H_
