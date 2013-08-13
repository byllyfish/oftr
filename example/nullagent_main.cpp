#include "ofp.h"
#include <iostream>

using namespace ofp;

 
static Features gFeatures{};

class NullAgent : public AbstractChannelListener {
public:
    void onChannelUp(Channel *channel) override {
        log::debug(__PRETTY_FUNCTION__);

        channel->openAuxChannel();
        channel->close();
    }
    
    void onMessage(const Message *message) override;

private:
    void onFeaturesRequest(const Message *message);
};


void NullAgent::onMessage(const Message *message)
{
    switch (message->type()) {
    case FeaturesRequest::Type:
        onFeaturesRequest(message);
        break;

    default:
        break;
    }
}

void NullAgent::onFeaturesRequest(const Message *message)
{
    FeaturesReplyBuilder msg{message};
    msg.setFeatures(gFeatures);
    msg.send(message->source());
}

int main(int argc, const char **argv)
{
    std::vector<std::string> args{argv + 1, argv + argc};

    IPv6Address addr{};
    if (!args.empty()) {
        addr = IPv6Address{args[0]};
    }

    Driver driver;

    auto lambda = []{
        return new NullAgent;
    };

    if (addr.valid()) {
        auto result = driver.connect(Driver::Agent, addr, Driver::DefaultPort, ProtocolVersions{}, lambda);
        result.get([](Exception ex) {
            std::cout << "Result: " << ex << '\n';
        });

    } else {
        driver.listen(Driver::Agent, IPv6Address{}, Driver::DefaultPort, ProtocolVersions{}, lambda);
    }

    driver.run();
}