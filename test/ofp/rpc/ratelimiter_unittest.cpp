#include "ofp/unittest.h"
#include "ofp/rpc/ratelimiter.h"

using namespace ofp;
using namespace ofp::rpc;


TEST(ratelimiter, test_2_4_10s) {
    RateLimiter limit{2, 4, TimeInterval{10, 0}};
    Timestamp now = Timestamp::now();

    std::vector<bool> actual;
    double events1[] = { 0.0, 2.5, 4.0, 5.0, 6.5, 9.5, 10.0 };
    for (auto ts: events1) {
        actual.push_back(limit.allow(now + TimeInterval{ts}));
    }

    std::vector<bool> expected = { true, true, false, false, true, true, false };
    EXPECT_EQ(actual, expected);
}

TEST(ratelimiter, test_2_6s) {
    RateLimiter limit{2, TimeInterval{6, 0}};
    Timestamp now = Timestamp::now();

    std::vector<bool> actual;
    double events1[] = { 0.0, 2.5, 4.0, 5.0, 6.5, 9.5, 10.0, 12.6 };
    for (auto ts: events1) {
        actual.push_back(limit.allow(now + TimeInterval{ts}));
    }

    std::vector<bool> expected = { true, true, false, false, true, true, false, true };
    EXPECT_EQ(actual, expected);
}

TEST(ratelimiter, test_2_3) {
    RateLimiter limit{2, 3};
    Timestamp now = Timestamp::now();

    std::vector<bool> actual;
    double events1[] = { 0.0, 2.5, 4.0, 5.0, 6.5, 9.5, 10.0, 12.6 };
    for (auto ts: events1) {
        actual.push_back(limit.allow(now + TimeInterval{ts}));
    }

    std::vector<bool> expected = { true, true, false, true, true, false, true, true };
    EXPECT_EQ(actual, expected);
}

TEST(ratelimiter, test_always) {
    RateLimiter limit{1, 1};
    Timestamp now = Timestamp::now();

    std::vector<bool> actual;
    double events1[] = { 0.0, 2.5, 4.0, 5.0, 6.5, 9.5, 10.0, 12.6 };
    for (auto ts: events1) {
        actual.push_back(limit.allow(now + TimeInterval{ts}));
    }

    std::vector<bool> expected = { true, true, true, true, true, true, true, true };
    EXPECT_EQ(actual, expected);
}

TEST(ratelimiter, test_never) {
    RateLimiter limit{0, 0xffffffff};
    Timestamp now = Timestamp::now();

    std::vector<bool> actual;
    double events1[] = { 0.0, 2.5, 4.0, 5.0, 6.5, 9.5, 10.0, 12.6 };
    for (auto ts: events1) {
        actual.push_back(limit.allow(now + TimeInterval{ts}));
    }

    std::vector<bool> expected = { false, false, false, false, false, false, false, false };
    EXPECT_EQ(actual, expected);
}
