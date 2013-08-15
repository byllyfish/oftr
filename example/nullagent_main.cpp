#include "ofp.h"
#include <iostream>
#include <vector>

using namespace ofp;

int main(int argc, const char **argv)
{
    std::vector<std::string> args{argv + 1, argv + argc};

    IPv6Address addr{};
    ProtocolVersions version{};
    if (!args.empty()) {
        addr = IPv6Address{args[0]};
        if (args.size() == 2) {
            int num = std::stoi(args[1]);
            version = ProtocolVersions{UInt8_narrow_cast(num)};
        }
    }

    Driver driver;

    if (addr.valid()) {
        auto result = driver.connect(Driver::Agent, addr, Driver::DefaultPort,
                                     version, NullAgent::Factory);
        result.done([](Exception ex) {
            std::cout << "Result: " << ex << '\n';
        });

    } else {
        auto result = driver.listen(Driver::Agent, IPv6Address{}, Driver::DefaultPort,
                                    version, NullAgent::Factory);

        result.done([](Exception ex) {
            std::cout << "Result: " << ex << '\n';
        });
    }

    driver.run();
}
