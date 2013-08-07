#include "ofp/log.h"
#include <iostream>

// TODO replace std::cerr with a way to set a logging ostream.


void ofp::log::info(const std::string &msg)
{
	std::cout << msg << '\n';
}
