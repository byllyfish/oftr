#include "ofp.h"

/*  -----  ofp::runDefaultController  -----  */

ofp::Exception
ofp::runDefaultController(ChannelListener::Factory listenerFactory)
{
    Driver driver;

    auto ex =
        driver.listen(Driver::Controller, IPv6Address{}, Driver::DefaultPort,
                      ProtocolVersions{}, listenerFactory);

    Exception result;
    ex.done([&result](Exception exc) {
        result = exc;
    });

    driver.run();

    return result;
}

/*  -----  ofp::runDefaultAgent  -----  */

ofp::Exception ofp::runDefaultAgent(const IPv6Address &remoteAddress,
                                    ChannelListener::Factory listenerFactory)
{
    Driver driver;

    auto ex = driver.connect(Driver::Agent, remoteAddress, Driver::DefaultPort,
                             ProtocolVersions{}, listenerFactory);

    Exception result;
    ex.done([&result](Exception exc) {
        result = exc;
    });

    driver.run();

    return result;
}
