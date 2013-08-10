#include "ofp/log.h"
#include <iostream>

// TODO replace std::cerr with a way to set a logging ostream.


void ofp::log::info(const std::string &msg)
{
	std::cerr << "[info] " << msg << '\n';
}


void ofp::log::debug(const std::string &msg)
{
	std::cerr << "[debug] " << msg << '\n';
}

/**
 *  Log when an API is used incorrectly.
 */
void ofp::log::exception(const std::string &msg)
{
	std::cerr << "[exception] " << msg << '\n';
}