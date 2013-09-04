//  ===== ---- ofp-c/ofpstring.h -----------------------------*- C -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Provides simple string api.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_C_OFPSTRING_H
#define OFP_C_OFPSTRING_H

#include "ofp/export.h"
#include <string.h>

extern "C" {

typedef struct {
    char *data;
    size_t length;
    size_t capacity;
} OFPString;

#define ofpStringNewEmpty()	{ NULL, 0, 0 }

OFP_EXPORT int ofpStringSet(OFPString *str, const void *data, size_t length);
OFP_EXPORT int ofpStringSetC(OFPString *str, const char *cstr);
OFP_EXPORT void ofpStringDispose(OFPString *str);
OFP_EXPORT void ofpStringClear(OFPString *str);
OFP_EXPORT int ofpStringEqualsC(OFPString *str, const char *cstr);

} // extern "C"

#endif // OFP_C_OFPSTRING_H