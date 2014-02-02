//  ===== ---- ofp/featuresreply.h -------------------------*- C++ -*- =====  //
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
/// \brief Defines the FeaturesReply and FeaturesReplyBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_FEATURESREPLY_H
#define OFP_FEATURESREPLY_H

#include "ofp/protocolmsg.h"
#include "ofp/padding.h"

namespace ofp {  // <namespace ofp>

/// \brief FeaturesReply is a concrete class for an OFPT_FEATURES_REPLY message.
class FeaturesReply : public ProtocolMsg<FeaturesReply, OFPT_FEATURES_REPLY> {
 public:
  const DatapathID &datapathId() const { return datapathId_; }
  UInt32 bufferCount() const { return bufferCount_; }
  UInt8 tableCount() const { return tableCount_; }
  UInt8 auxiliaryId() const { return auxiliaryId_; }
  UInt32 capabilities() const { return capabilities_; }
  UInt32 reserved() const { return reserved_; }

  //PortRange ports() const;

  bool validateLength(size_t length) const;

 private:
  Header header_;
  DatapathID datapathId_;
  Big32 bufferCount_ = 0;
  Big8 tableCount_ = 0;
  Big8 auxiliaryId_ = 0;
  Padding<2> pad_;
  Big32 capabilities_ = 0;
  Big32 reserved_ = 0;

  // Only FeaturesReplyBuilder can construct an actual instance.
  FeaturesReply() : header_{type()} {}

  friend class FeaturesReplyBuilder;
};

static_assert(sizeof(FeaturesReply) == 32, "Unexpected size.");
static_assert(IsStandardLayout<FeaturesReply>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<FeaturesReply>(),
              "Expected trivially copyable.");

/// \brief FeaturesReplyBuilder is a concrete class for building an
/// OFPT_FEATURES_REPLY message.
class FeaturesReplyBuilder {
 public:
  explicit FeaturesReplyBuilder(UInt32 xid);
  explicit FeaturesReplyBuilder(const FeaturesReply *msg);

  void setDatapathId(const DatapathID &datapathId) {
    msg_.datapathId_ = datapathId;
  }
  void setBufferCount(UInt32 bufferCount) { msg_.bufferCount_ = bufferCount; }
  void setTableCount(UInt8 tableCount) { msg_.tableCount_ = tableCount; }
  void setAuxiliaryId(UInt8 auxiliaryId) { msg_.auxiliaryId_ = auxiliaryId; }
  void setCapabilities(UInt32 capabilities) {
    msg_.capabilities_ = capabilities;
  }
  void setReserved(UInt32 reserved) { msg_.reserved_ = reserved; }

  UInt32 send(Writable *channel);

 private:
  FeaturesReply msg_;
  //PortList ports_;
};

}  // </namespace ofp>

#endif  // OFP_FEATURESREPLY_H
