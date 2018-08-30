// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_LIBOFP_H_
#define OFP_LIBOFP_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LIBOFP_EXPORT __attribute__((visibility("default")))
#define LIBOFP_USED __attribute__((used))
	
#define OFTR_VERSION 0
#define OFTR_ENCODE_OPENFLOW 1
#define OFTR_DECODE_OPENFLOW 2

/// Invoke a library function synchronously.
///
/// The result is copied into the specified output buffer and the size of the
/// result is returned.
///
/// Params:
///   opcode: Of this 32-bit quantity, the least significant byte specifies
///      the operation (opcode & 0xFF). Depending on the operation, the other
///      bits may represent flags or OF version.
///       0 = get sw version  [ 0x0 | 0x0 | 0x0 | 0x0 ]
///       1 = encode-openflow [ OF-version | 0x0 | 0x0 | 0x1 ]
///       2 = decode-openflow [ 0x0 | 0x0 | 0x0 | 0x02 ]
///   input: Input buffer
///   input_len: Input buffer size
///   output: Mutable output buffer
///   output_len: Output buffer size
///
/// Returns:
///   >= 0           Actual size of output buffer (must be <= output_len)
///   -1             Error: Invalid arguments
///   -2             Error: Internal error
///   <= -3          Error: Take absolute value to obtain size of output buffer
///                         with error message (<= output_len).
///   < -output_len  Error: Take absolute value to obtain necessary output
///                         buffer size (> output_len).
///
/// Limits:
///    Input and output buffer sizes must be <= 2^30-1 (1 GB).
///
/// Note:
///    Text output is NOT zero-terminated.

LIBOFP_EXPORT int32_t oftr_call(uint32_t opcode, const char *input,
                                size_t input_len, char *output,
                                size_t output_len) LIBOFP_USED;

#ifdef __cplusplus
}  // extern C
#endif

#endif  // OFP_LIBOFP_H_
