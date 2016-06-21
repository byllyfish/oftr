// Copyright (c) 2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/log.h"
#include "ofp/unittest.h"

static int always_signal = 0;

static int always_call(int x) {
  always_signal = x;
  return x;
}

static int never_call(int x) {
  ofp::log::fatal("never_call()", x);
}

TEST(log, test_debug) {
  auto savedFilter = ofp::log::detail::GLOBAL_OutputLevelFilter;
  ofp::log::setOutputLevelFilter(ofp::log::Level::Debug);

#if defined(NDEBUG)
  log_debug("the answer is", never_call(11));
#else
  log_debug("the answer is", always_call(12));
  EXPECT_EQ(12, always_signal);

  ofp::log::setOutputLevelFilter(ofp::log::Level::Info);
  log_debug("the answer is", never_call(13));
#endif

  ofp::log::setOutputLevelFilter(savedFilter);
}

TEST(log, test_error) {
  auto savedFilter = ofp::log::detail::GLOBAL_OutputLevelFilter;

  ofp::log::setOutputLevelFilter(ofp::log::Level::Error);
  log_error("the answer is", always_call(101));
  EXPECT_EQ(101, always_signal);

  ofp::log::setOutputLevelFilter(ofp::log::Level::Fatal);
  log_error("the answer is", never_call(102));

  ofp::log::setOutputLevelFilter(savedFilter);
}

TEST(log, test_warning) {
  auto savedFilter = ofp::log::detail::GLOBAL_OutputLevelFilter;

  ofp::log::setOutputLevelFilter(ofp::log::Level::Warning);
  log_warning("the answer is", always_call(201));
  EXPECT_EQ(201, always_signal);

  ofp::log::setOutputLevelFilter(ofp::log::Level::Fatal);
  log_warning("the answer is", never_call(202));

  ofp::log::setOutputLevelFilter(savedFilter);
}

TEST(log, test_info) {
  auto savedFilter = ofp::log::detail::GLOBAL_OutputLevelFilter;

  ofp::log::setOutputLevelFilter(ofp::log::Level::Info);
  log_info("the answer is", always_call(301));
  EXPECT_EQ(301, always_signal);

  ofp::log::setOutputLevelFilter(ofp::log::Level::Fatal);
  log_info("the answer is", never_call(302));

  ofp::log::setOutputLevelFilter(savedFilter);
}

TEST(log, dangling_else) {
  auto savedFilter = ofp::log::detail::GLOBAL_OutputLevelFilter;

  ofp::log::setOutputLevelFilter(ofp::log::Level::Error);

  if (true)
    log_error("the answer is", always_call(11));

  EXPECT_EQ(11, always_signal);

  ofp::log::setOutputLevelFilter(ofp::log::Level::Fatal);

  if (true)
    log_error("the answer is", never_call(13));
  else
    never_call(14);

  ofp::log::setOutputLevelFilter(savedFilter);
}
