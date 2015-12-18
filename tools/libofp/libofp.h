
// Experimental C API -- This API is a placeholder to be revisited.
// 
// Example 1: libofp_encode
// 
//     libofp_buffer input = { 0, 0 };
//     input.data = "type: HELLO\nversion: 4";
//     input.length = strlen(input.data);
//     
//     libofp_buffer result;
//     if (libofp_encode(&result, &input, 0) < 0) {
//         printf("error: %s", result.data);
//     } else {
//         /* do something with result */
//     }
//     libofp_buffer_free(&result);
//     
// Example 2: libofp_version
// 
//     libofp_buffer result = { 0, 0 };
//     libofp_version(&result);
//     printf("version: %s", result.data);
//     libofp_buffer_free(&result);
// 
// The original contents of the mutable `result` buffer are never used. Its
// value is always overwritten with a newly allocated pointer. You must always 
// call `libofp_buffer_free` to free the buffer when you are done -- even when 
// there is an error.
// 
// It is a good idea to initialize the result buffer to { 0, 0 }. The API may
// change in the future to allow for a pre-allocated result buffer. A null
// buffer will signal that the result buffer should be allocated.

#ifndef LIBOFP_H_
#define LIBOFP_H_

#include <stddef.h>
#include <stdint.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define LIBOFP_EXPORT   __attribute__((visibility("default")))

typedef struct {
    char *data;
    size_t length;
} libofp_buffer;

/// Retrieve the library version number.
/// 
/// Client is responsible for calling `libofp_buffer_free` on result buffer.
LIBOFP_EXPORT void libofp_version(libofp_buffer *result);

/// Translate a YAML string to a binary OpenFlow message. 
/// 
/// Returns 0 if there is no error.
/// Returns -1 and result contains error message if there is an error.
/// 
/// Client is responsible for calling `libofp_buffer_free` on result buffer.
LIBOFP_EXPORT int libofp_encode(libofp_buffer *result, const libofp_buffer *input, uint32_t flags);

/// Translate a binary OpenFlow message to YAML.
/// 
/// Returns 0 if there is no error.
/// Returns -1 and result contains error message if there is an error.
/// 
/// Client is responsible for calling `libofp_buffer_free` on result buffer.
LIBOFP_EXPORT int libofp_decode(libofp_buffer *result, const libofp_buffer *input, uint32_t flags);

/// Dispose of `buffer` object returned by library functions.
LIBOFP_EXPORT void libofp_buffer_free(libofp_buffer *buffer);

#ifdef  __cplusplus
}  /* extern C */
#endif

#endif /* LIBOFP_H_ */


