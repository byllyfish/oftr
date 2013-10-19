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

static Deferred<int> startImmediately(int value) 
{
	auto result = Deferred<int>::makeResult();
	result->done(value);
	return result;
}

TEST(deferred, lifetimeTest1) 
{
	{
		auto result = start();

		result.done([](int value) {
			EXPECT_EQ(1, value);
		});

		saveForLater->done(1);
		saveForLater = nullptr;
	}
}


TEST(deferred, lifetimeTest2) 
{
	{
		auto result = start();

		result.done([](int value) {
			EXPECT_EQ(2, value);
		});
	}

	saveForLater->done(2);
	saveForLater = nullptr;
}

TEST(deferred, lifetimeTest3) 
{
	{
		auto result = startUnsaved();

		result.done([](int value) {
			// Must never be called.
			EXPECT_TRUE(false);
		});
	}
}


TEST(deferred, lifetimeTest4) 
{
	{
		auto result = start();

		saveForLater->done(-2);

		result.done([](int value) {
			EXPECT_EQ(-2, value);
		});

		// Must not be called again.
		saveForLater->done(4);
	}

	saveForLater = nullptr;
}


TEST(deferred, testImmediateCall) 
{
	{
		auto result = startImmediately(5);

		result.done([](int value) {
			EXPECT_EQ(5, value);
		});

		result.done([](int value) {
			// Must never be called.
			EXPECT_TRUE(false);
		});
	}
}


static Deferred<int> startImmediatelyReturn(int value) 
{
	return value;
}


TEST(deferred, testImmediateReturn)
{
	auto result = startImmediatelyReturn(7);

	result.done([](int value) {
		EXPECT_EQ(7, value);
	});

	result.done([](int value) {
		// Must never be called.
		EXPECT_TRUE(false);
	});
}
