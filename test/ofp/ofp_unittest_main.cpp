// Copyright 2014-present Bill Fisher. All rights reserved.

#include "gtest/gtest.h"
#include "ofp/log.h"

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);

  // The key thing is to set up logging to stderr before running the tests.
  ofp::log::setOutputStream(&std::clog);
  ofp::log::setOutputLevelFilter(ofp::log::Level::Debug);

  return RUN_ALL_TESTS();
}
