#include "ofp/unittest.h"
#include "ofp/driver.h"

using namespace ofp;


class MockChannelListener : public ChannelListener {
public:
	void onChannelUp(Channel *channel) override {}
	void onChannelDown() override {}
	void onMessage(const Message *message) override {}
	void onError(const Error *error) override {}
};


TEST(driver, test) 
{
	Driver driver;

	driver.listen(Driver::Controller, Driver::DefaultPort, []{
		return new MockChannelListener;
	});

	driver.run();
}
