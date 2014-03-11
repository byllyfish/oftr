#include "ofp/ofp.h"
#include <iostream>
#include <chrono>

using namespace ofp;

static void flowMod(Writable *channel, UInt32 inPort, UInt32 bufferId,
                    const EnetAddress &dst, const EnetAddress &src,
                    UInt32 outPort) {
  MatchBuilder match;
  match.add(OFB_IN_PORT{inPort});
  match.add(OFB_ETH_DST{dst});
  match.add(OFB_ETH_SRC{src});

  ActionList actions;
  actions.add(AT_OUTPUT{outPort});

  InstructionList instructions;
  instructions.add(IT_APPLY_ACTIONS{&actions});

  FlowModBuilder flowMod;
  flowMod.setMatch(match);
  flowMod.setInstructions(instructions);
  flowMod.setIdleTimeout(10);
  flowMod.setHardTimeout(30);
  flowMod.setPriority(0x8000);
  flowMod.setBufferId(bufferId);
  flowMod.send(channel);
}

int main(int argc, char **argv) {
  UInt32 inPort = 1;
  UInt32 outPort = 2;
  UInt32 bufferId = 249;
  EnetAddress dst{"11-22-33-44-55-66"};
  EnetAddress src{"11-77-88-99-AA-BB"};

  MemoryChannel channel{OFP_VERSION_4};

  using clock = std::chrono::high_resolution_clock;
  using milliseconds = std::chrono::milliseconds;

  const unsigned loops = 500000;

  for (int trial = 1; trial <= 10; ++trial) {
      auto start = clock::now();

      for (unsigned i = 0; i < loops; ++i) {
        flowMod(&channel, inPort, bufferId, dst, src, outPort);
        channel.clear();
      }

      auto end = clock::now();
      auto duration = std::chrono::duration_cast<milliseconds>(end - start);
      std::cout << "Loops=" << loops << " Trial " << trial << " Time: " << duration.count() << " ms\n";
  }

  return 0;
}
