
#include "bridgelistener.h"
#include <iostream>

using namespace bridge;

int main()
{
    Driver driver;
    IPv6Address remoteAddr{"192.168.56.1"};

    log::set(&std::cerr);

    driver.listen(Driver::Bridge, nullptr, IPv6Address{}, Driver::DefaultPort,
                  ProtocolVersions::All, [remoteAddr]() { return new BridgeListener(remoteAddr); });

    driver.run();

    return 0;
}