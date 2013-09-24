//  ===== ---- ofp-c/ofpstring.h -----------------------------*- C -*- =====  //
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