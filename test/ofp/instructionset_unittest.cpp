// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/instructionlist.h"
#include "ofp/protocollist.h"
#include "ofp/unittest.h"

using namespace ofp;

static void MakeInstructionList(InstructionList &) {}

template <class Type, class... Args>
void MakeInstructionList(InstructionList &set, Type value, Args... args) {
  set.add(value);
  MakeInstructionList(set, args...);
}

template <class Type, class... Args>
InstructionList MakeInstructions(Type value, Args... args) {
  InstructionList set;
  MakeInstructionList(set, value, args...);
  return set;
}

template <class Head, class... Tail>
void MakeInstructionListFromTuple(InstructionList &set,
                                  std::tuple<Head, Tail...> tuple) {
  set.add(std::get<0>(tuple));
  MakeInstructionListFromTuple(set, tuple);
}

template <class Head, class... Tail>
InstructionList MakeInstructionsFromTuple(std::tuple<Head, Tail...> tuple) {
  InstructionList set;
  MakeInstructionListFromTuple(tuple);
  return set;
}

TEST(instructionlist, test) {
  InstructionList list;

  list.add(IT_GOTO_TABLE{5});
  list.add(IT_CLEAR_ACTIONS{});

  EXPECT_EQ(16, list.size());

  auto expected = "0001-0008-05-000000  0005-0008-00000000";
  EXPECT_HEX(expected, list.data(), list.size());

  auto iter = list.begin();
  auto iterEnd = list.end();

  EXPECT_EQ(IT_GOTO_TABLE::type(), iter->type());
  EXPECT_EQ(8, iter.size());

  ++iter;
  EXPECT_EQ(IT_CLEAR_ACTIONS::type(), iter->type());
  EXPECT_EQ(8, iter.size());

  ++iter;
  EXPECT_EQ(iterEnd, iter);
}

TEST(instructionlist, MakeInstructions) {
  auto set = MakeInstructions(IT_GOTO_TABLE{5}, IT_CLEAR_ACTIONS{});

  auto expected = "0001-0008-05-000000  0005-0008-00000000";
  EXPECT_HEX(expected, set.data(), set.size());
}

TEST(instructionlist, MakeInstructionList) {
  InstructionList set;
  MakeInstructionList(set, IT_GOTO_TABLE{5}, IT_CLEAR_ACTIONS{});

  auto expected = "0001-0008-05-000000  0005-0008-00000000";
  EXPECT_HEX(expected, set.data(), set.size());
}
