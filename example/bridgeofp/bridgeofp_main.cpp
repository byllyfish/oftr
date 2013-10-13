
#include "bridgelistener.h"
#include <boost/program_options.hpp>
#include <iostream>

using namespace bridge;
using namespace std;

namespace po = boost::program_options;

using IPv6Pair = pair<IPv6Endpoint,IPv6Endpoint>;

namespace std {
istream &operator>>(istream &is, IPv6Pair &pair);
ostream &operator<<(ostream &os, const IPv6Pair &pair);
}

int main(int argc, char **argv)
{
    try {
        log::set(&std::cerr);

        IPv6Pair endpoints;
    	po::options_description usage{"General Usage"};
        usage.add_options()
            ("help,h", "produce help message")
            ("listen,L", po::value<IPv6Pair>(&endpoints)->required(), "listen and bridge to remote endpoint");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, usage), vm);
        if (vm.count("help")) {
            cout << usage << '\n';
            return 1;
        }
        po::notify(vm); 

        Driver driver;

        auto exc = driver.listen(Driver::Bridge, nullptr, endpoints.first,
                      ProtocolVersions::All, [endpoints]() {
            return new BridgeListener(endpoints.second);
        });

        int exitCode = 0;
        exc.done([&exitCode](Exception ex) {
            if (ex) {
                cerr << "ERROR: " << ex << '\n';
                exitCode = 2;
            }
        });

        driver.run();

        return exitCode;

    } catch (std::exception &ex) {
        cerr << "ERROR: " << ex.what() << '\n';
        return 2;
    }
}

namespace std {
istream &operator>>(istream &is, IPv6Pair &pair)
{
    return is >> skipws >> pair.first >> pair.second;
}

ostream &operator<<(ostream &os, const IPv6Pair &pair)
{
    return os << pair.first << ' ' << pair.second;
}
}
