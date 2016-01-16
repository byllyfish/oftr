// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_SYS_DTLS_UTILS_H_
#define OFP_SYS_DTLS_UTILS_H_

#include "ofp/types.h"

namespace ofp {
namespace sys {

/// \brief Get length of first DTLS record in given buffer.
///
/// The record length includes the record header. If there is no recognizable
/// DTLS record, or the calculated record length extends past the end of the
/// buffer, return 0.
///
/// The minimum record length is DTLS1_RT_HEADER_LENGTH (13 bytes).

size_t DTLS_GetRecordLength(const void *buffer, size_t length);

/// \brief Print a one-line human-readable description of the DTLS record.
///
/// The output is suitable for debug logging and tracing.

std::string DTLS_PrintRecord(const void *record, size_t length);

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_DTLS_UTILS_H_
