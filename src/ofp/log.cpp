#include "ofp/log.h"
#include <iostream>

// TODO replace std::cerr with a way to set a logging ostream.


void ofp::log::write(const char *type, const std::string &msg)
{
	std::cerr << type << msg << '\n';
}
