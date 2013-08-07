#include "ofp/unittest.h"
#include "ofp/instructionset.h"

using namespace ofp;

#if 0
template <class T1, class T2>
InstructionSet MakeInstructions(T1 v1, T2 v2) {
	InstructionSet set;
	set.add(v1);
	set.add(v2);
	return set;
}
#endif

static void MakeInstructionSet(InstructionSet &) {}

template <class Type, class... Args>
void MakeInstructionSet(InstructionSet &set, Type value, Args... args)
{
	set.add(value);
	MakeInstructionSet(set, args...);
}

template <class Type, class... Args>
InstructionSet MakeInstructions(Type value, Args... args) {
	InstructionSet set;
	MakeInstructionSet(set, value, args...);
	return set;
}

//
//
template <class Head, class... Tail>
void MakeInstructionSetFromTuple(InstructionSet &set, std::tuple<Head, Tail...> tuple) {
	set.add(std::get<0>(tuple));
	MakeInstructionSetFromTuple(set, tuple);
}

template <class Head, class... Tail>
InstructionSet MakeInstructionsFromTuple(std::tuple<Head, Tail...> tuple) {
	InstructionSet set;
	MakeInstructionSetFromTuple(tuple);
	return set;
}




// use std::tuple?  std::forward_as_tuple
// 
// 



TEST(instructionset, test)
{
    InstructionSet set;

    set.add(IT_GOTO_TABLE{5});
    set.add(IT_CLEAR_ACTIONS{});

    EXPECT_EQ(16, set.size());

    auto expected = "0001-0008-05-000000  0005-0008-00000000";
    EXPECT_HEX(expected, set.data(), set.size());
}


TEST(instructionset, MakeInstructions) {
	auto set = MakeInstructions(IT_GOTO_TABLE{5}, IT_CLEAR_ACTIONS{});

    auto expected = "0001-0008-05-000000  0005-0008-00000000";
    EXPECT_HEX(expected, set.data(), set.size());

}


TEST(instructionset, MakeInstructionSet) {
	InstructionSet set;
	MakeInstructionSet(set, IT_GOTO_TABLE{5}, IT_CLEAR_ACTIONS{});

    auto expected = "0001-0008-05-000000  0005-0008-00000000";
    EXPECT_HEX(expected, set.data(), set.size());
}

#if 0
TEST(instructionset, MakeInstructionSetFromTuple) {
	InstructionSet set;
	MakeInstructionSetFromTuple(set, { IT_GOTO_TABLE{5}, IT_CLEAR_ACTIONS{}} );

    auto expected = "0001-0008-05-000000  0005-0008-00000000";
    EXPECT_HEX(expected, set.data(), set.size());
}
#endif


TEST(instructionset, ostreamlike) {
	InstructionSet set;

#if 0
	set << { IT_GOTO_TABLE{5}, IT_CLEAR_ACTIONS{} };

 	auto m = Match{
 		OFB_IN_PORT{4}, 
 		OFB_TCP_DST{80}
 	};

 	auto i = Instructions{
 		IT_GOTO_TABLE{5},
 		IT_WRITE_ACTIONS{
 			AT_PUSH_VLAN{6},
 			AT_SET_NW_TTL{25}
 		}
 	};

 	auto fm = FlowMod{m, i, priority};

 	send(fm);
 #endif
}

