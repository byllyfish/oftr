//  ===== ---- ofp-c/ofpprotocol.h ---------------------------*- C -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Provides simple protocol api.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_C_OFPPROTOCOL_H
#define OFP_C_OFPPROTOCOL_H

#include "ofp-c/ofpstring.h"

extern "C" {

OFP_EXPORT int ofpProtocolEncode(OFPString *inMessage, OFPString *outBinary,
                                 OFPString *outError);

OFP_EXPORT int ofpProtocolDecode(OFPString *inBinary, OFPString *outMessage,
                                 OFPString *outError);

} // extern "C"

#endif // OFP_C_OFPPROTOCOL_H
