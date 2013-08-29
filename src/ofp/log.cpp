#include "ofp/log.h"
#include <ctime>

namespace ofp { // <namespace ofp>
namespace log { // <namespace log>

using milliseconds = std::chrono::milliseconds;

static std::ostream *GlobalLogStream = nullptr;

std::ostream *get() {
	return GlobalLogStream;
}

void set(std::ostream *logStream) {
	GlobalLogStream = logStream;
}

using Time = std::pair<std::time_t, milliseconds>;

static Time currentTime()
{
	using namespace std::chrono;

	auto now = system_clock::now();
	milliseconds ms = duration_cast<milliseconds>(now.time_since_epoch());
	time_t t = system_clock::to_time_t(now);

	return { t, ms % 1000 };
}


static std::ostream &operator<<(std::ostream &os, const Time &t)
{
	return os << t.first << '.' << t.second.count();
}

void write(const char *type, const std::string &msg)
{
	if (GlobalLogStream) {
		*GlobalLogStream << currentTime() << ' ' << type << ' ' << msg << '\n';
	}
}

} // </namespace log>
} // </namespace ofp>