#include "ofp.h"

//  -----  ofp::runController  ----- 

ofp::Exception ofp::runController(ChannelListener::Factory listenerFactory,
                                  ProtocolVersions versions)
{
    Driver driver;

    auto ex = driver.listen(Driver::Controller, nullptr, IPv6Address{},
                            Driver::DefaultPort, versions, listenerFactory);

    Exception result;
    ex.done([&result](Exception exc) {
        result = exc;
    });

    driver.run();

    return result;
}

//  -----  ofp::runAgent  -----

ofp::Exception ofp::runAgent(const Features &features, const IPv6Address &remoteAddress,
                             ChannelListener::Factory listenerFactory,
                             ProtocolVersions versions)
{
    Driver driver;

    auto ex = driver.connect(Driver::Agent, &features, remoteAddress, Driver::DefaultPort,
                             versions, listenerFactory);

    Exception result;
    ex.done([&result](Exception exc) {
        result = exc;
    });

    driver.run();

    return result;
}
