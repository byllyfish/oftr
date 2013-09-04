//  ===== ---- ofp/unittest.h ------------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines useful functions and classes for unit tests.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_UNITTEST_H
#define OFP_UNITTEST_H

#include <gtest/gtest.h>
#include "ofp/types.h"
#include "ofp/memorychannel.h"
#include <cstdio>

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wglobal-constructors"
#endif

inline std::string hexclean(const char *data)
{
    std::string s = ofp::HexToRawData(data);
    return ofp::RawDataToHex(s.data(), s.size());
}

#define EXPECT_HEX(hexstr, data, length)                                       \
    {                                                                          \
        auto hex_tmp__ = hexclean(hexstr);                                     \
        EXPECT_EQ(hex_tmp__, ofp::RawDataToHex(data, length));                 \
    }

#endif // OFP_UNITTEST_H
