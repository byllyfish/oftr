#include "ofp/unittest.h"
#include "ofp/driver.h"
#include "ofp/exception.h"

using namespace ofp;

class MockChannelListener : public ChannelListener {
public:
    void onChannelUp(Channel *channel) override
    {
    }
    void onChannelDown(Channel *channel) override
    {
    }
    void onMessage(const Message *message) override;

    void onException(const Exception *error) override
    {
    }
    void onTimer(UInt32 timerID) override {
    }
};


void MockChannelListener::onMessage(const Message *message)
{
}


TEST(driver, test)
{
    log::set(&std::cerr);
    
    Driver driver;

    driver.listen(Driver::Controller, nullptr, IPv6Endpoint{OFP_DEFAULT_PORT}, 
                  ProtocolVersions{}, []{
        return new MockChannelListener;
    });

    //driver.run();
}
