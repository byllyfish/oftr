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

// Arguments of the LOG_ macros are only evaluated at the given log level.

#define LOG_IF_LEVEL_(lvl_, ...)                                              \
  if (::ofp::log::Level::lvl_ < ::ofp::log::detail::GLOBAL_OutputLevelFilter) \
    0;                                                                        \
  else                                                                        \
  ::ofp::log::detail::write_(::ofp::log::Level::lvl_, __VA_ARGS__)

#define LOG_ERROR(...) LOG_IF_LEVEL_(Error, __VA_ARGS__)
// #define LOG_WARNING(...)  LOG_IF_LEVEL_(Warning, __VA_ARGS__)
// #define LOG_INFO(...)     LOG_IF_LEVEL_(Info, __VA_ARGS__)

#if defined(NDEBUG)
#define LOG_DEBUG(...) 0
#else
#define LOG_DEBUG(...) LOG_IF_LEVEL_(Debug, __VA_ARGS__)
#endif

TEST(log, test_debug) {
  auto savedFilter = ofp::log::detail::GLOBAL_OutputLevelFilter;
  ofp::log::setOutputLevelFilter(ofp::log::Level::Debug);

#if defined(NDEBUG)
  LOG_DEBUG("the answer is", never_call(11));
#else
  LOG_DEBUG("the answer is", always_call(12));
  EXPECT_EQ(12, always_signal);

  ofp::log::setOutputLevelFilter(ofp::log::Level::Info);
  LOG_DEBUG("the answer is", never_call(13));
#endif

  ofp::log::setOutputLevelFilter(savedFilter);
}

TEST(log, test_error) {
  auto savedFilter = ofp::log::detail::GLOBAL_OutputLevelFilter;

  ofp::log::setOutputLevelFilter(ofp::log::Level::Error);
  LOG_ERROR("the answer is", always_call(101));
  EXPECT_EQ(101, always_signal);

  ofp::log::setOutputLevelFilter(ofp::log::Level::Fatal);
  LOG_ERROR("the answer is", never_call(102));

  ofp::log::setOutputLevelFilter(savedFilter);
}
