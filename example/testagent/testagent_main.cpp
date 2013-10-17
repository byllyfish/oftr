#include "ofp.h"
#include "testagent.h"
#include <iostream>

using namespace testagent;

int main(int argc, char **argv)
{
	Features features{};

	IPv6Address remoteAddr{"127.0.0.1"};

	//log::set(&std::cerr);

	Exception err = ofp::runAgent(features, remoteAddr, TestAgent::Factory, {OFP_VERSION_1});


	return 0;
}