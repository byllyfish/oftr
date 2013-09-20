#include "ofp/log.h"
#include "ofp/yaml/apiserver.h"
#include <iostream>

using namespace ofp;
using namespace yaml;

int main(int argc, char **argv)
{
	// TODO daemonize, support command-line arguments...

	log::set(&std::cerr);

	ApiServer::run(IPv6Address{}, 9191);

	return 0;
}
