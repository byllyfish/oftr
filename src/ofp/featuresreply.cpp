//  ===== ---- ofp/featuresreply.cpp -----------------------*- C++ -*- =====  //
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
/// \brief Implements FeaturesReply and FeaturesReplyBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/featuresreply.h"
#include "ofp/channel.h"
#include "ofp/log.h"

namespace ofp { // <namespace ofp>

void FeaturesReply::getFeatures(Features *features) const {
  features->setDatapathId(datapathId_);
  features->setBufferCount(bufferCount_);
  features->setTableCount(tableCount_);
  features->setAuxiliaryId(auxiliaryId_);
  features->setCapabilities(capabilities_);
  features->setReserved(reserved_);
}

bool FeaturesReply::validateLength(size_t length) const {
  if (length < sizeof(FeaturesReply)) {
    log::info("FeatureReply validateLength failed.");
    return false;
  }

  // TODO check number of ports in reply if version < 4.

  return true;
}

FeaturesReplyBuilder::FeaturesReplyBuilder(const FeaturesReply *msg)
    : msg_{*msg} {}

FeaturesReplyBuilder::FeaturesReplyBuilder(UInt32 xid) {
  // Set xid of reply to request's xid.
  msg_.header_.setXid(xid);
}

void FeaturesReplyBuilder::setFeatures(const Features &features) {
  msg_.datapathId_ = features.datapathId();
  msg_.bufferCount_ = features.bufferCount();
  msg_.tableCount_ = features.tableCount();
  msg_.auxiliaryId_ = features.auxiliaryId();
  msg_.capabilities_ = features.capabilities();
  msg_.reserved_ = features.reserved();
}

UInt32 FeaturesReplyBuilder::send(Writable *channel) {
  msg_.header_.setVersion(channel->version());
  msg_.header_.setLength(sizeof(msg_));

  channel->write(&msg_, sizeof(msg_));
  channel->flush();

  return msg_.header_.xid();
}

} // </namespace ofp>
