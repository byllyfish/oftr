// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "gtest/gtest.h"
#include "ofp/log.h"

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);

// The key thing is to set up logging to stderr before running the tests.
#if !defined(NDEBUG)
  ofp::log::configure(ofp::log::Level::Debug);
#else
  ofp::log::configure(ofp::log::Level::Fatal);
#endif

  return RUN_ALL_TESTS();
}
