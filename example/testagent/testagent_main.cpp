#include "ofp.h"
#include "testagent.h"
#include <iostream>

using namespace testagent;

int main(int argc, char **argv) {
  IPv6Address remoteAddr{"127.0.0.1"};

  // log::set(&std::cerr);

  std::error_code err =
      ofp::runAgent(remoteAddr, TestAgent::Factory, {OFP_VERSION_1});

  if (err) {
    std::cerr << "Error starting agent: " << err << '\n';
    return 1;
  }

  return 0;
}
