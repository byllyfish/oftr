#ifndef OFP_LOGLEVEL_H_
#define OFP_LOGLEVEL_H_

#include "ofp/types.h"

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

const char *levelToString(Level level);

/// Type of function called when an event is logged. There can be at most one
/// output callback function specified at any one time.
typedef void (*OutputCallback)(Level level, const char *line, size_t size,
                               void *context);

/// \brief Sets the output callback function. This function will be called for
/// each line of log output with the specified context pointer.
void setOutputCallback(OutputCallback callback, void *context);

/// \brief Sets the output callback to log each line to the specified output
/// stream (e.g. std::cerr).
void setOutputStream(std::ostream *outputStream);

/// \brief Sets the minimum desired level of output.
void setOutputLevelFilter(Level level);

}  // namespace log
}  // namespace ofp

#endif // OFP_LOGLEVEL_H_
