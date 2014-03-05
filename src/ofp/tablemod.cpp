//  ===== ---- ofp/tablemod.cpp ----------------------------*- C++ -*- =====  //
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
/// \brief Implements the TableMod and TableModBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/tablemod.h"
#include "ofp/writable.h"

using namespace ofp;

bool TableMod::validateInput(size_t length) const {
  if (length != sizeof(TableMod)) {
    return false;
  }

  return true;
}

TableModBuilder::TableModBuilder(const TableMod *msg) : msg_{*msg} {}

UInt32 TableModBuilder::send(Writable *channel) {
  UInt32 xid = channel->nextXid();
  UInt8 version = channel->version();
  size_t msgLen = sizeof(TableMod);

  msg_.header_.setVersion(version);
  msg_.header_.setLength(UInt16_narrow_cast(msgLen));
  msg_.header_.setXid(xid);

  channel->write(&msg_, sizeof(msg_));
  channel->flush();

  return xid;
}
