#include "ofp.h"
#include "simplechannellistener.h"
#include <iostream>

using namespace ofp;

int main(int argc, char **argv)
{
	log::set(&std::cerr);

	auto factory = []() {
        return new controller::SimpleChannelListener;
    };

    auto ex = runController(factory);

    if (ex) {
    	log::error("Error running controller:", ex);
        return 1;
    }

    return 0;
}

