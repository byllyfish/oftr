// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include "ofp/ofp.h"

using namespace ofp;

static void flowMod(Writable *channel, UInt32 inPort, UInt32 bufferId,
                    const MacAddress &dst, const MacAddress &src,
                    UInt32 outPort) {
  FlowModBuilder flowMod;

  MatchBuilder &match = flowMod.match();
  match.add(OFB_IN_PORT{inPort});
  match.add(OFB_ETH_DST{dst});
  match.add(OFB_ETH_SRC{src});

  ActionList actions;
  actions.add(AT_OUTPUT{outPort});

  InstructionList &instructions = flowMod.instructions();
  instructions.add(IT_APPLY_ACTIONS{&actions});

  flowMod.setIdleTimeout(10);
  flowMod.setHardTimeout(30);
  flowMod.setPriority(0x8000);
  flowMod.setBufferId(bufferId);
  flowMod.send(channel);
}

static void logStats(const std::vector<int64_t> &data) {
  if (data.size() == 0)
    return;

  double sum = std::accumulate(data.begin(), data.end(), 0);
  double mean = sum / data.size();

  double accum = 0.0;
  std::for_each(data.begin(), data.end(), [mean, &accum](uint64_t x) {
    accum += (x - mean) * (x - mean);
  });

  double stddev = std::sqrt(accum / data.size());

  std::cout << "Time Mean/Stddev: " << mean << "/" << stddev << '\n';
}

int main(int argc, char **argv) {
  UInt32 inPort = 1;
  UInt32 outPort = 2;
  MacAddress dst{"11-22-33-44-55-66"};
  MacAddress src{"11-77-88-99-AA-BB"};

  MemoryChannel channel{OFP_VERSION_4};

  using clock = std::chrono::high_resolution_clock;
  using microseconds = std::chrono::microseconds;

  const int kTrials = 5;
  const unsigned kLoops = 1000000;
  std::vector<int64_t> results;

  std::cout << "Running FlowMod Benchmark: " << kTrials << " trials, " << kLoops
            << " loops each" << std::endl;

  for (int trial = 0; trial <= kTrials; ++trial) {
    auto start = clock::now();

    for (unsigned i = 0; i < kLoops; ++i) {
      flowMod(&channel, inPort, i, dst, src, outPort);
      channel.clear();
    }

    auto end = clock::now();

    microseconds duration =
        std::chrono::duration_cast<microseconds>(end - start);
    std::cout << "Loops=" << kLoops << " Trial " << std::setw(2) << trial
              << " Time: " << duration.count() << " usec" << std::endl;

    // Ignore trial 0.
    if (trial == 0) {
      std::cout << "Ignore trial 0..." << std::endl;
      continue;
    }

    results.push_back(duration.count());
  }

  logStats(results);

  return 0;
}
