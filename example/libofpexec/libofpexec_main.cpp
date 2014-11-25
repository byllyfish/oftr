// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/ofp.h"
#include "ofp/api/apiserver.h"
#include <iostream>

using namespace ofp;

const int STDIN = 0;
const int STDOUT = 1;

int main(int argc, char **argv) {
  log::setOutputStream(&std::clog);

  Driver driver;
  api::ApiServer server{&driver, STDIN, STDOUT};

  driver.run();

  return 0;
}
