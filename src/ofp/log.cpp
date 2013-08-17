#include "ofp/log.h"

namespace ofp { // <namespace ofp>
namespace log { // <namespace log>

static std::ostream *GlobalLogStream = nullptr;

std::ostream *get() {
	return GlobalLogStream;
}

void set(std::ostream *logStream) {
	GlobalLogStream = logStream;
}

void write(const char *type, const std::string &msg)
{
	if (GlobalLogStream) {
		*GlobalLogStream << type << msg << '\n';
	}
}

} // </namespace log>
} // </namespace ofp>