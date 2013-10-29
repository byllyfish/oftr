//  ===== ---- ofp/ofpstring.cpp ---------------------------*- C++ -*- =====  //
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
/// \brief Implements string functions for C API.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp-c/ofpstring.h"
#include <stdlib.h>
#include <assert.h>

/// Initializes a NULL-terminated string with the specified data. If provided
/// length is negative, uses the C `strlen` function to determine the length of
/// the data.
/// \param outDest ptr to string object.
/// \param data    ptr to string data.
/// \param length  length of string data, or negative for `strlen`.
int ofpStringSet(OFPString *str, const void *data, size_t length) {
  assert(str);

  if (str->data == NULL) {
    // String is unallocated.
    str->length = length;
    str->capacity = length + 1;
    str->data = (char *)malloc(str->capacity);
    if (str->data == NULL) {
      str->capacity = 0;
      str->length = 0;
      return 0;
    }
    if (data)
      memcpy(str->data, data, length);
    str->data[length] = 0;
    return 1;
  }

  if (str->capacity > length + 1) {
    // String already has capacity for new string.
    str->length = length;
    if (data)
      memcpy(str->data, data, length);
    str->data[length] = 0;
    return 1;
  }

  // String is allocated but capacity is too small.
  char *ptr = (char *)realloc(str->data, length + 1);
  if (ptr == NULL) {
    return 0;
  }

  str->capacity = length + 1;
  str->length = length;
  str->data = ptr;
  if (data)
    memcpy(str->data, data, length);
  str->data[length] = 0;

  return 1;
}

int ofpStringSetC(OFPString *str, const char *cstr) {
  int ret = ofpStringSet(str, NULL, strlen(cstr));
  if (ret) {
    memcpy(str->data, cstr, str->length);
    return 1;
  }

  return 0;
}

void ofpStringDispose(OFPString *str) {
  assert(str);

  if (str->capacity > 0 && str->data != NULL) {
    free(str->data);
  }

  memset(str, 0, sizeof(OFPString));
}

void ofpStringClear(OFPString *str) { ofpStringDispose(str); }

int ofpStringEqualsC(OFPString *str, const char *cstr) {
  assert(str);
  assert(cstr);

  if (str->data == NULL) {
    return cstr[0] == 0;
  }

  return strcmp(str->data, cstr) == 0;
}