//  ===== ---- ofp/features.h ------------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
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
    Features(const DatapathID &datapathID, UInt32 bufferCount = 1,
             UInt8 tableCount = 1)
        : datapathID_{datapathID}, bufferCount_{bufferCount},
          tableCount_{tableCount}
    {
    }

    DatapathID datapathID() const
    {
        return datapathID_;
    }

    UInt32 bufferCount() const
    {
        return bufferCount_;
    }

    UInt8 tableCount() const
    {
        return tableCount_;
    }

    UInt8 auxiliaryID() const
    {
        return auxiliaryID_;
    }

    UInt32 capabilities() const
    {
        return capabilities_;
    }

    UInt32 reserved() const
    {
        return reserved_;
    }

    void setDatapathID(DatapathID datapathID)
    {
        datapathID_ = datapathID;
    }

    void setBufferCount(UInt32 bufferCount)
    {
        bufferCount_ = bufferCount;
    }

    void setTableCount(UInt8 tableCount)
    {
        tableCount_ = tableCount;
    }

    void setAuxiliaryID(UInt8 auxiliaryID)
    {
        auxiliaryID_ = auxiliaryID;
    }

    void setCapabilities(UInt32 capabilities)
    {
        capabilities_ = capabilities;
    }

    void setReserved(UInt32 reserved)
    {
        reserved_ = reserved;
    }

private:
    DatapathID datapathID_;
    UInt32 bufferCount_ = 0;
    UInt8 tableCount_ = 0;
    UInt8 auxiliaryID_ = 0;
    Padding<2> pad_;
    UInt32 capabilities_ = 0;
    UInt32 reserved_ = 0;
};

} // </namespace ofp>

#endif // OFP_FEATURES_H
