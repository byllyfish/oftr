// Copyright 2014-present Bill Fisher. All rights reserved.

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
  Trace = 1,
  Info = 2,
  Warning = 3,
  Error = 4,
  Fatal = 5,
  Silent = 6
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

#if defined(LIBOFP_TARGET_DARWIN)

/// \brief Sets the output callback to log each line via Apple System Logging.
void setOutputStream(aslclient outputStream);

#endif  // defined(LIBOFP_TARGET_DARWIN)

/// \brief Sets the minimum desired level of output.
void setOutputLevelFilter(Level level);

}  // namespace log
}  // namespace ofp

#endif  // OFP_LOGLEVEL_H_
