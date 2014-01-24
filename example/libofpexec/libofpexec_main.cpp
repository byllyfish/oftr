//  ===== ---- ofp/libofpexec_main.cpp ---------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Implements program to run an ApiServer.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp.h"
#include "ofp/yaml/apiserver.h"
#include <iostream>

using namespace ofp;

const int STDIN = 0;
const int STDOUT = 1;

int main(int argc, char **argv) {
  log::set(&std::cerr);
  
  Driver driver;
  yaml::ApiServer server{&driver, STDIN, STDOUT};

  driver.run();

  return 0;
}
