//  ===== ---- ofp/log.h -----------------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Implements logging hooks for the library.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_LOG_H
#define OFP_LOG_H

#include <string>
#include <sstream>

namespace ofp { // <namespace ofp>
namespace log { // <namespace log>

/// \returns the global log output stream or nullptr if none is set.
std::ostream *get();

/// \brief Sets the global log output stream.
/// Set `logStream` to nullptr to disable logging entirely.
void set(std::ostream *logStream);

void write(const char *type, const std::string &msg);

template <class Type>
void write(const char *type, const std::string &msg, const Type &value) {
    std::stringstream ss;
    ss << msg;
    ss << ' ';
    ss << value;
    write(type, ss.str());
}

void info(const std::string &msg);


template <class Type>
void info(const std::string &msg, const Type &value)
{
	write("[info] ", msg, value);
}

void debug(const std::string &msg);

template <class Type>
void debug(const std::string &msg, const Type &value)
{
    write("[debug]", msg, value);
}

void error(const std::string &msg);

template <class Type>
void error(const std::string &msg, const Type &value) {
	write("[error] ", msg, value);
}

void exception(const std::string &msg);


class Lifetime {
public:
	 Lifetime(const char *description) : description_{description}
	 {
	 	debug("Create ", description_);
	 }

	~Lifetime() {
		debug("Dispose ", description_);
	}

private:
	const char *description_;
};


inline void info(const std::string &msg)
{
	write("[info] ", msg);
}


inline void debug(const std::string &msg) {
	write("[debug]", msg);
}


inline void error(const std::string &msg) {
	write("[error] ", msg);
}


inline void exception(const std::string &msg)
{
	write("[exception] ", msg);
}


} // </namespace log>
} // </namespace ofp>

#endif // OFP_LOG_H
