// Copyright (c) 2016-2017 William W. Fisher (at gmail dot com)
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

using namespace ofp::log;

TEST(log, test_debug) {
  auto savedFilter = GLOBAL_Logger->levelFilter();
  GLOBAL_Logger->setLevelFilter(Level::Debug);

#if defined(NDEBUG)
  log_debug("the answer is", never_call(11));
#else
  log_debug("the answer is", always_call(12));
  EXPECT_EQ(12, always_signal);

  GLOBAL_Logger->setLevelFilter(Level::Info);
  log_debug("the answer is", never_call(13));
#endif

  GLOBAL_Logger->setLevelFilter(savedFilter);
}

TEST(log, test_error) {
  auto savedFilter = GLOBAL_Logger->levelFilter();

  GLOBAL_Logger->setLevelFilter(Level::Error);
  log_error("the answer is", always_call(101));
  EXPECT_EQ(101, always_signal);

  GLOBAL_Logger->setLevelFilter(Level::Fatal);
  log_error("the answer is", never_call(102));

  GLOBAL_Logger->setLevelFilter(savedFilter);
}

TEST(log, test_warning) {
  auto savedFilter = GLOBAL_Logger->levelFilter();

  GLOBAL_Logger->setLevelFilter(Level::Warning);
  log_warning("the answer is", always_call(201));
  EXPECT_EQ(201, always_signal);

  GLOBAL_Logger->setLevelFilter(Level::Fatal);
  log_warning("the answer is", never_call(202));

  GLOBAL_Logger->setLevelFilter(savedFilter);
}

TEST(log, test_info) {
  auto savedFilter = GLOBAL_Logger->levelFilter();

  GLOBAL_Logger->setLevelFilter(Level::Info);
  log_info("the answer is", always_call(301));
  EXPECT_EQ(301, always_signal);

  GLOBAL_Logger->setLevelFilter(Level::Fatal);
  log_info("the answer is", never_call(302));

  GLOBAL_Logger->setLevelFilter(savedFilter);
}

TEST(log, dangling_else) {
  auto savedFilter = GLOBAL_Logger->levelFilter();

  GLOBAL_Logger->setLevelFilter(Level::Error);

  if (true)
    log_error("the answer is", always_call(11));

  EXPECT_EQ(11, always_signal);

  GLOBAL_Logger->setLevelFilter(ofp::log::Level::Fatal);

  if (true)
    log_error("the answer is", never_call(13));
  else
    never_call(14);

  GLOBAL_Logger->setLevelFilter(savedFilter);
}

static int log_crash_illegal() {
  volatile int *ptr = 0;
  return *ptr;
}

static size_t log_crash_stack_overflow(size_t n) {
  if (n > 0xffffffff) return 0;
  return log_crash_stack_overflow(n + 1);
}

OFP_BEGIN_IGNORE_USED_BUT_UNUSED

TEST(log_DeathTest, log_crash_illegal) {
  // This macro generates a warning on clang. [-Wused-but-marked-unused]
  EXPECT_EXIT(log_crash_illegal(), ::testing::KilledBySignal(SIGILL), "36log_DeathTest_log_crash_illegal_Test8TestBody");
}

TEST(log_DeathTest, log_crash_stack_overflow) {
  // This macro generates a warning on clang. [-Wused-but-marked-unused]
  EXPECT_EXIT(log_crash_stack_overflow(0), ::testing::KilledBySignal(SIGILL), "28log_DeathTest_log_crash_Test8TestBody");
}

OFP_END_IGNORE_USED_BUT_UNUSED

