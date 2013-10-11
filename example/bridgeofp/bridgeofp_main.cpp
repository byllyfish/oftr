
#include "bridgelistener.h"
#include <iostream>

using namespace bridge;

int main()
{
    Driver driver;
    IPv6Endpoint remoteEndpoint{"192.168.56.1", OFP_DEFAULT_PORT};

    log::set(&std::cerr);

    driver.listen(Driver::Bridge, nullptr, IPv6Endpoint{OFP_DEFAULT_PORT},
                  ProtocolVersions::All, [remoteEndpoint]() {
        return new BridgeListener(remoteEndpoint);
    });

    driver.run();

    return 0;
}