#include "ofp/unittest.h"
#include "ofp/deferred.h"

using namespace ofp;

#if defined(__clang__)
# pragma clang diagnostic ignored "-Wexit-time-destructors"
#endif

static ofp::DeferredResultPtr<int> saveForLater = nullptr;

static Deferred<int> start() 
{
	auto result = Deferred<int>::makeResult();
	saveForLater = result;
	return result;
}

static Deferred<int> startUnsaved()
{
	return Deferred<int>::makeResult();
}

TEST(deferred, lifetimeTest1) 
{
	{
		auto result = start();

		result.get([](int value) {
			std::cout << "Result set to " << value << '\n';
		});

		saveForLater->set(1);
		saveForLater = nullptr;
	}
}


TEST(deferred, lifetimeTest2) 
{
	{
		auto result = start();

		result.get([](int value) {
			std::cout << "Result set to " << value << '\n';
		});
	}

	saveForLater->set(2);
	saveForLater = nullptr;
}

TEST(deferred, lifetimeTest3) 
{
	{
		auto result = startUnsaved();

		result.get([](int value) {
			std::cout << "Result set to " << value << '\n';
		});
	}
}


TEST(deferred, lifetimeTest4) 
{
	{
		auto result = start();

		// This set is lost.
		saveForLater->set(-2);

		result.get([](int value) {
			std::cout << "Result set to " << value << '\n';
		});

		saveForLater->set(4);
	}

	saveForLater = nullptr;
}
