#include "ofp.h"
#include <iostream>

using namespace ofp;

static Features gFeatures{};

class NullAgent : public AbstractChannelListener {
public:

    static NullAgent *Factory() { return new NullAgent; }

    void onChannelUp(Channel *channel) override
    {
        log::debug(__PRETTY_FUNCTION__);
    }

    void onMessage(const Message *message) override;

private:
    void onFeaturesRequest(const Message *message);
    void sendError(const Message *message, UInt16 type, UInt16 code);
};

void NullAgent::onMessage(const Message *message)
{
    switch (message->type()) {
    case FeaturesRequest::Type:
        onFeaturesRequest(message);
        break;

    default:
        sendError(message, 1, 1);
        break;
    }
}

void NullAgent::onFeaturesRequest(const Message *message)
{
    FeaturesReplyBuilder msg{message};
    msg.setFeatures(gFeatures);
    msg.send(message->source());
}

void NullAgent::sendError(const Message *message, UInt16 type, UInt16 code)
{
    ErrorBuilder msg{type, code};
    msg.send(message->source());
}

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
        result.get([](Exception ex) {
            std::cout << "Result: " << ex << '\n';
        });

    } else {
        driver.listen(Driver::Agent, IPv6Address{}, Driver::DefaultPort,
                      version, NullAgent::Factory);
    }

    driver.run();
}
