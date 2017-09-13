// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "gtest/gtest.h"
#include "ofp/log.h"

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);

#if !defined(NDEBUG)
  // The key thing is to set up logging to stderr before running the tests.
  ofp::log::configure(ofp::log::Level::Debug);
#endif 
  
  return RUN_ALL_TESTS();
}
