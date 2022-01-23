// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/yaml/ymatch.h"

#include "ofp/unittest.h"
#include "ofp/yaml/outputjson.h"

using namespace ofp;

static void diagnosticHandler(const llvm::SMDiagnostic &diag, void *context) {
  llvm::errs() << diag.getMessage().str() << '\n';
}

TEST(ymatch, decodeMatchYaml) {
  MatchBuilder match;

  std::string input{
      R"""(---
- field: IN_PORT
  value: 1
...
)"""};

  llvm::yaml::Input yin{input, nullptr, diagnosticHandler, nullptr};
  yin >> match;

  EXPECT_FALSE(yin.error());

  OXMRange fields = match.toRange();
  EXPECT_GT(fields.size(), 0);

  OXMIterator iter = fields.begin();
  EXPECT_EQ(OFB_IN_PORT::type(), iter->type());

  OFB_IN_PORT inPort = iter->value<OFB_IN_PORT>();
  EXPECT_EQ(1, inPort.value());

  ++iter;
  EXPECT_EQ(fields.end(), iter);
}

TEST(ymatch, decodeMatchYamlPortNumber) {
  MatchBuilder match;

  std::string input{
      R"""(---
- field: IN_PORT
  value: CONTROLLER
...
)"""};

  llvm::yaml::Input yin{input, nullptr, diagnosticHandler, nullptr};
  yin >> match;

  EXPECT_FALSE(yin.error());

  OXMRange fields = match.toRange();
  EXPECT_GT(fields.size(), 0);

  OXMIterator iter = fields.begin();
  EXPECT_EQ(OFB_IN_PORT::type(), iter->type());

  OFB_IN_PORT inPort = iter->value<OFB_IN_PORT>();
  EXPECT_EQ(OFPP_CONTROLLER, inPort.value());

  ++iter;
  EXPECT_EQ(fields.end(), iter);
}

TEST(ymatch, decodeMatchCaseSensitive) {
  MatchBuilder match;

  std::string input{
      R"""(---
- field: in_port
  value: 1
...
)"""};

  llvm::yaml::Input yin{input, nullptr, diagnosticHandler, nullptr};
  yin >> match;

  EXPECT_TRUE(yin.error());
}
