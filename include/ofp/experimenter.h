//  ===== ---- ofp/experimenter.h --------------------------*- C++ -*- =====  //
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
/// \brief Defines the Experimenter class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_EXPERIMENTER_H
#define OFP_EXPERIMENTER_H

#include "ofp/protocolmsg.h"
#include "ofp/bytelist.h"

namespace ofp { // <namespace ofp>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //
//   E x p e r i m e n t e r
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //
/// \brief Implements immutable Experimenter protocol message.

class Experimenter : public ProtocolMsg<Experimenter,OFPT_EXPERIMENTER> {
public:

    UInt32 experimenter() const
    {
        return experimenter_;
    }
    
    UInt32 expType() const
    {
        return expType_;
    }

    ByteRange expData() const;

    bool validateLength(size_t length) const;

private:
    Header header_;
    Big32 experimenter_;
    Big32 expType_;

    // Only ExperimenterBuilder can construct an actual instance.
    Experimenter() : header_{type()}
    {
    }

    friend class ExperimenterBuilder;
};

static_assert(sizeof(Experimenter) == 16, "Unexpected size.");
static_assert(IsStandardLayout<Experimenter>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<Experimenter>(), "Expected trivially copyable.");

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //
//   E x p e r i m e n t e r B u i l d e r
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //
/// \brief Implements Experimenter protocol message builder.

class ExperimenterBuilder {
public:

    ExperimenterBuilder() {}

    explicit ExperimenterBuilder(const Experimenter *msg);

    void setExperimenter(UInt32 experimenter)
    {
        msg_.experimenter_ = experimenter;
    }

    void setExpType(UInt32 expType)
    {
        msg_.expType_ = expType;
    }

    void setExpData(const void *data, size_t length) {
        data_.set(data, length);
    }

    UInt32 send(Writable *channel);

private:
    Experimenter msg_;
    ByteList data_;
};

} // </namespace ofp>

#endif // OFP_EXPERIMENTER_H
