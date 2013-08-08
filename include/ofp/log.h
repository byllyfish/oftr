#ifndef OFP_LOG_H
#define OFP_LOG_H

#include <string>
#include <sstream>


namespace ofp { // <namespace ofp>

namespace log { // <namespace log>


void info(const std::string &msg);

template <class Type>
void info(const std::string &msg, Type value) 
{
	std::stringstream ss;
	ss << msg;
	ss << value;
	info(ss.str());
}


void exception(const std::string &msg);


} // </namespace log>

} // </namespace ofp>

#endif // OFP_LOG_H
