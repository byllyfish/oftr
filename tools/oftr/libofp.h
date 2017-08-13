// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.
//
// Experimental C API -- This API is a placeholder to be revisited.
//
// Example 1: libofp_encode
//
//     const char *yaml_input = "type: HELLO\nversion: 4";
//     libofp_buffer result = { nullptr, 0 };
//     if (libofp_encode(&result, yaml, 0) < 0) {
//         printf("error: %s", result.data);
//     } else {
//         /* do something with result */
//     }
//     libofp_buffer_free(&result);
//
// Example 2: libofp_version
//
//     libofp_buffer result = { nullptr, 0 };
//     libofp_version(&result);
//     printf("version: %s", result.data);
//     libofp_buffer_free(&result);
//
// The original contents of the mutable `result` buffer are ignored. The buffer
// value is always overwritten with a newly allocated pointer. You must always
// call `libofp_buffer_free` to free the buffer when you are done -- even when
// there is an error.

#ifndef TOOLS_OFTR_LIBOFP_H_
#define TOOLS_OFTR_LIBOFP_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LIBOFP_EXPORT __attribute__((visibility("default")))
#define LIBOFP_USED __attribute__((used))

typedef struct {
  void *data;
  size_t length;
} libofp_buffer;

/// Retrieve the library version number.
///
/// Client is responsible for calling `libofp_buffer_free` on result buffer.
LIBOFP_EXPORT void libofp_version(libofp_buffer *result) LIBOFP_USED;

/// Translate a YAML string to a binary OpenFlow message.
///
/// Returns 0 if there is no error.
/// Returns -1 and result contains error message if there is an error.
///
/// Client is responsible for calling `libofp_buffer_free` on result buffer.
LIBOFP_EXPORT int libofp_encode(libofp_buffer *result, const char *yaml_input,
                                uint32_t flags) LIBOFP_USED;

/// Translate a binary OpenFlow message to YAML.
///
/// Returns 0 if there is no error.
/// Returns -1 and result contains error message if there is an error.
///
/// Client is responsible for calling `libofp_buffer_free` on result buffer.
LIBOFP_EXPORT int libofp_decode(libofp_buffer *result,
                                const libofp_buffer *buffer,
                                uint32_t flags) LIBOFP_USED;

/// Dispose of `buffer` object returned by library functions.
LIBOFP_EXPORT void libofp_buffer_free(libofp_buffer *buffer) LIBOFP_USED;

#ifdef __cplusplus
}  // extern C
#endif

#endif  // TOOLS_OFTR_LIBOFP_H_
