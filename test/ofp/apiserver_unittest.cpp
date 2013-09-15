#include "ofp/unittest.h"
#include "ofp/driver.h"
#include "ofp/yaml/apiserver.h"

using namespace ofp;
using namespace yaml;

TEST(apiserver, test)
{
	log::set(&std::cerr);

	//ApiServer::run(IPv6Address{}, 9191);
}
