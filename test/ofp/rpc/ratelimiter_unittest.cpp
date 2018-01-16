// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/rpc/ratelimiter.h"
#include "ofp/unittest.h"

using namespace ofp;
using namespace ofp::rpc;

TEST(ratelimiter, test_2_4_10s) {
  RateLimiter limit{2, 4, TimeInterval{10, 0}};
  Timestamp now = Timestamp::now();

  std::vector<bool> actual;
  double events1[] = {0.0, 2.5, 4.0, 5.0, 6.5, 9.5, 10.0};
  for (auto ts : events1) {
    actual.push_back(limit.allow(now + TimeInterval{ts}));
  }

  std::vector<bool> expected = {true, true, false, false, true, true, false};
  EXPECT_EQ(actual, expected);
}

TEST(ratelimiter, test_2_6s) {
  RateLimiter limit{2, TimeInterval{6, 0}};
  Timestamp now = Timestamp::now();

  std::vector<bool> actual;
  double events1[] = {0.0, 2.5, 4.0, 5.0, 6.5, 9.5, 10.0, 12.6};
  for (auto ts : events1) {
    actual.push_back(limit.allow(now + TimeInterval{ts}));
  }

  std::vector<bool> expected = {true, true, false, false,
                                true, true, false, true};
  EXPECT_EQ(actual, expected);
}

TEST(ratelimiter, test_2_3) {
  RateLimiter limit{2, 3};
  Timestamp now = Timestamp::now();

  std::vector<bool> actual;
  double events1[] = {0.0, 2.5, 4.0, 5.0, 6.5, 9.5, 10.0, 12.6};
  for (auto ts : events1) {
    actual.push_back(limit.allow(now + TimeInterval{ts}));
  }

  std::vector<bool> expected = {true, true,  false, true,
                                true, false, true,  true};
  EXPECT_EQ(actual, expected);
}

TEST(ratelimiter, test_always) {
  RateLimiter limit{true};
  Timestamp now = Timestamp::now();

  std::vector<bool> actual;
  double events1[] = {0.0, 2.5, 4.0, 5.0, 6.5, 9.5, 10.0, 12.6};
  for (auto ts : events1) {
    actual.push_back(limit.allow(now + TimeInterval{ts}));
  }

  std::vector<bool> expected = {true, true, true, true, true, true, true, true};
  EXPECT_EQ(actual, expected);
}

TEST(ratelimiter, test_never) {
  RateLimiter limit{false};
  Timestamp now = Timestamp::now();

  std::vector<bool> actual;
  double events1[] = {0.0, 2.5, 4.0, 5.0, 6.5, 9.5, 10.0, 12.6};
  for (auto ts : events1) {
    actual.push_back(limit.allow(now + TimeInterval{ts}));
  }

  std::vector<bool> expected = {false, false, false, false,
                                false, false, false, false};
  EXPECT_EQ(actual, expected);
}

TEST(ratelimiter, test_500_1s) {
  RateLimiter limit{500, TimeInterval{1.0}};
  Timestamp now = Timestamp::now();

  double ts = 0.0;
  size_t count = 0;
  for (unsigned i = 0; i < 5000; ++i) {
    if (limit.allow(now + TimeInterval{ts})) {
      ++count;
    }
    ts += 0.001;
  }

  EXPECT_EQ(count, 2500);
}

TEST(ratelimiter, test_not_a_token_bucket) {
  RateLimiter limit{5, TimeInterval{10.0}};
  Timestamp now = Timestamp::now();

  std::vector<bool> actual;
  double events1[] = {0.0,  9.9,  9.9,  9.9,  9.9, 10.0,
                      10.0, 10.0, 10.0, 10.0, 10.0};
  for (auto ts : events1) {
    actual.push_back(limit.allow(now + TimeInterval{ts}));
  }

  std::vector<bool> expected = {true, true, true, true, true, true,
                                true, true, true, true, false};
  EXPECT_EQ(actual, expected);
}

TEST(ratelimiter, copy) {
  RateLimiter limit{5, TimeInterval{10.0}};
  RateLimiter empty{false};

  EXPECT_EQ(empty.n(), 0);
  empty = limit;
  EXPECT_EQ(empty.n(), 5);
}

TEST(ratelimiter, parse_valid) {
  RateLimiter limit{false};

  EXPECT_TRUE(limit.parse("true"));
  EXPECT_EQ(limit.toString(), "true");

  EXPECT_TRUE(limit.parse("false"));
  EXPECT_EQ(limit.toString(), "false");

  EXPECT_TRUE(limit.parse("1/2.0"));
  EXPECT_EQ(limit.toString(), "1/2.000000000");

  EXPECT_TRUE(limit.parse("1:2"));
  EXPECT_EQ(limit.toString(), "1:2");

  EXPECT_TRUE(limit.parse("1:2/3.0"));
  EXPECT_EQ(limit.toString(), "1:2/3.000000000");
}

TEST(ratelimiter, parse_invalid) {
  RateLimiter limit{false};

  EXPECT_FALSE(limit.parse("1/2.0x"));
  EXPECT_FALSE(limit.parse(" 1/2.0"));
  EXPECT_FALSE(limit.parse("1:2x"));
  EXPECT_FALSE(limit.parse("1:2/3.0x"));
  EXPECT_FALSE(limit.parse(""));
  EXPECT_FALSE(limit.parse(" "));
  EXPECT_FALSE(limit.parse("x:2/3.0"));
  EXPECT_FALSE(limit.parse("1:x/3.0"));
  EXPECT_FALSE(limit.parse("1:2/x"));
}
