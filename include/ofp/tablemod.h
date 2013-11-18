//  ===== ---- ofp/tablemod.h ------------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the TableMod and TableModBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_TABLEMOD_H
#define OFP_TABLEMOD_H

#include "ofp/protocolmsg.h"
#include "ofp/padding.h"

namespace ofp { // <namespace ofp>

class TableMod : public ProtocolMsg<TableMod, OFPT_TABLE_MOD, 16, 16> {
public:
  UInt8 tableId() const { return tableId_; }
  UInt32 config() const { return config_; }

  bool validateLength(size_t length) const;

private:
  Header header_;
  Big8 tableId_;
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

  void setTableId(UInt8 tableId) { msg_.tableId_ = tableId; }
  void setConfig(UInt32 config) { msg_.config_ = config; }

  UInt32 send(Writable *channel);

private:
  TableMod msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

} // </namespace ofp>

#endif // OFP_TABLEMOD_H
