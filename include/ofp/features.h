//  ===== ---- ofp/features.h ------------------------------*- C++ -*- =====  //
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
/// \brief Defines the Features class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_FEATURES_H
#define OFP_FEATURES_H

#include "ofp/padding.h"
#include "ofp/datapathid.h"

namespace ofp { // <namespace ofp>

/// \brief Represents the features of a switch. {7.3.1}
// FIXME Need to handle version 1!
class Features {
public:
  Features() = default;
  Features(const DatapathID &datapathId, UInt32 bufferCount = 1,
           UInt8 tableCount = 1)
      : datapathId_{datapathId}, bufferCount_{bufferCount},
        tableCount_{tableCount} {}

  DatapathID datapathId() const { return datapathId_; }

  UInt32 bufferCount() const { return bufferCount_; }

  UInt8 tableCount() const { return tableCount_; }

  UInt8 auxiliaryId() const { return auxiliaryId_; }

  UInt32 capabilities() const { return capabilities_; }

  UInt32 reserved() const { return reserved_; }

  void setDatapathId(DatapathID datapathId) { datapathId_ = datapathId; }

  void setBufferCount(UInt32 bufferCount) { bufferCount_ = bufferCount; }

  void setTableCount(UInt8 tableCount) { tableCount_ = tableCount; }

  void setAuxiliaryId(UInt8 auxiliaryId) { auxiliaryId_ = auxiliaryId; }

  void setCapabilities(UInt32 capabilities) { capabilities_ = capabilities; }

  void setReserved(UInt32 reserved) { reserved_ = reserved; }

private:
  DatapathID datapathId_;
  UInt32 bufferCount_ = 0;
  UInt8 tableCount_ = 0;
  UInt8 auxiliaryId_ = 0;
  Padding<2> pad_;
  UInt32 capabilities_ = 0;
  UInt32 reserved_ = 0;
};

} // </namespace ofp>

#endif // OFP_FEATURES_H
