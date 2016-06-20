// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_LOGLEVEL_H_
#define OFP_LOGLEVEL_H_

#if defined(LIBOFP_TARGET_DARWIN)
#include <asl.h>
#endif  // defined(LIBOFP_TARGET_DARWIN)
#include "ofp/types.h"

namespace llvm {
class raw_ostream;
}  // namespace llvm

namespace ofp {
namespace log {

enum class Level {
  Debug = 0,
  Info = 1,
  Warning = 2,
  Error = 3,
  Fatal = 4,
  Silent = 5,
  Trace = 6
};

enum class Trace {
  Msg = 0,
  Rpc = 1,
};

/// Type of function called when an event is logged. There can be at most one
/// output callback function specified at any one time.
typedef void (*OutputCallback)(Level level, const char *line, size_t size,
                               void *context);

/// \brief Sets the output callback function. This function will be called for
/// each line of log output with the specified context pointer.
void setOutputCallback(OutputCallback callback, void *context);

/// \brief Sets the output callback to log each line to the specified output
/// stream (e.g. std::clog).
void setOutputStream(std::ostream *outputStream);

/// \brief Sets the output callback to log each line to the specified output
/// stream, specified as a raw_ostream.
void setOutputStream(llvm::raw_ostream *outputStream);

/// \brief Sets the minimum desired level of output.
void setOutputLevelFilter(Level level);

/// \brief Sets the desired trace options for output.
void setOutputTraceFilter(UInt32 trace);

}  // namespace log
}  // namespace ofp

#endif  // OFP_LOGLEVEL_H_
