#ifndef OFP_LOG_H
#define OFP_LOG_H

#include <string>


namespace ofp { // <namespace ofp>

namespace log { // <namespace log>


void info(const std::string &msg)
{
	std::cout << msg << '\n';
}

template <class Type>
void info(const std::string &msg, Type value) 
{
	std::cout << msg << " " << value << '\n';
}

template <>
void info(const std::string &msg, const std::string &param)
{
	std::cout << msg << " " << param << '\n';
}

} // </namespace log>

} // </namespace ofp>

#endif // OFP_LOG_H
