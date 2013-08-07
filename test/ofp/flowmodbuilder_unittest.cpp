#include <ofp/unittest.h>
#include "ofp/flowmodbuilder.h"
#include "ofp/matchbuilder.h"
#include "ofp/instructions.h"
//#include "ofp/"
using namespace ofp;

TEST(flowmodbuilder, test) 
{
	MatchBuilder match;
	match.add(OFB_IN_PORT{27});

	InstructionSet instructions;
	instructions.add(IT_GOTO_TABLE{3});

	FlowModBuilder flowMod;
	flowMod.setMatch(match);
	flowMod.setInstructions(instructions);

	auto s = flowMod.serialize();
	EXPECT_HEX("", s.data(), s.size());
}
