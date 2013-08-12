#include "ofp.h"
#include <iostream>

using namespace ofp;


class NullController : public AbstractChannelListener {
public:

    void onChannelUp(Channel *channel) override {
        log::debug(__PRETTY_FUNCTION__);
    }
    
    void onMessage(const Message *message) override
    {
    }

    static ChannelListener *Factory() { return new NullController; }
};

int main(int argc, char **argv)
{
   std::vector<std::string> args{argv + 1, argv + argc};

    IPv6Address addr{};
    if (!args.empty()) {
        addr = IPv6Address{args[0]};
    }

	Driver driver;

    if (addr.valid()) {
        auto result = driver.connect(Driver::Controller, addr, Driver::DefaultPort,
                  ProtocolVersions{}, NullController::Factory);

        result.get([](Exception ex) {
            std::cout << "Result " << ex << '\n';
        });

    } else {
        driver.listen(Driver::Controller, IPv6Address{}, Driver::DefaultPort, ProtocolVersions{}, NullController::Factory);
    }

    driver.run();
}