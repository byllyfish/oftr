//  ===== ---- ofp/log.h -----------------------------------*- C++ -*- =====  //
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
/// \brief Implements logging hooks for the library.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_LOG_H
#define OFP_LOG_H

#include "ofp/types.h"
#include <string>
#include <sstream>

namespace ofp { // <namespace ofp>
namespace log { // <namespace log>

/// \returns the global log output stream or nullptr if none is set.
std::ostream *get();

/// \brief Sets the global log output stream.
/// Set `logStream` to nullptr to disable logging entirely.
void set(std::ostream *logStream);

void write(const std::string &msg);

template <class Type1>
void write(const char *type, const Type1 &a)
{
    std::stringstream ss;
    ss << type;
    ss << ' ';
    ss << a;
    write(ss.str());
}

template <class Type1, class Type2>
void write(const char *type, const Type1 &a, const Type2 &b)
{
    std::stringstream ss;
    ss << type;
    ss << ' ';
    ss << a;
    ss << ' ';
    ss << b;
    write(ss.str());
}

inline void trace(const char *type, const void *data, size_t length)
{
    write(type, RawDataToHex(data, length));
}

template <class Type1>
void info(const Type1 &a)
{
    write("[info]", a);
}

template <class Type1, class Type2>
void info(const Type1 &a, const Type2 &b)
{
    write("[info]", a, b);
}

template <class Type1>
void debug(const Type1 &a)
{
    //write("[debug]", a);
}

template <class Type1, class Type2>
void debug(const Type1 &a, const Type2 &b)
{
    //write("[debug]", a, b);
}

template <class Type1>
void error(const Type1 &a)
{
    write("[error]", a);
}

template <class Type1, class Type2>
void error(const Type1 &a, const Type2 &b)
{
    write("[error]", a, b);
}

class Lifetime {
public:
    Lifetime(const char *description) : description_{description}
    {
        debug("Create ", description_);
    }

    ~Lifetime()
    {
        debug("Dispose ", description_);
    }

private:
    const char *description_;
};

} // </namespace log>
} // </namespace ofp>

#endif // OFP_LOG_H
