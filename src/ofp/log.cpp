//  ===== ---- ofp/log.cpp ---------------------------------*- C++ -*- =====  //
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
/// \brief Implements logging functions.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/log.h"
#include <chrono>
//#include <ctime>

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

void write(const std::string &msg)
{
	if (GlobalLogStream) {
		*GlobalLogStream << currentTime() << ' ' << msg << '\n';
	}
}

} // </namespace log>
} // </namespace ofp>
