#include "ofp/ofp.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <numeric>
#include <cmath>

using namespace ofp;

static uint64_t mallocCount = 0;
static uint64_t mallocSize = 0;

void *operator new(size_t size) {
  ++mallocCount;
  mallocSize += size;
  return malloc(size);
}

//

void *operator new [](size_t size) {
  ++mallocCount;
  mallocSize += size;
  return malloc(size);
} 

//

void operator delete(void *p) noexcept {
  free(p);
}

// 
void operator delete [](void *p) noexcept {
  free(p);
}

//

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
  flowMod.setMatch(std::move(match));
  flowMod.setInstructions(std::move(instructions));
  flowMod.setIdleTimeout(10);
  flowMod.setHardTimeout(30);
  flowMod.setPriority(0x8000);
  flowMod.setBufferId(bufferId);
  flowMod.send(channel);
}

static void logStats(const std::vector<int64_t> &data) {
  if (data.size() == 0) return;

  double sum = std::accumulate(data.begin(), data.end(), 0);
  double mean = sum / data.size();

  double accum = 0.0;
  std::for_each(data.begin(), data.end(),
                [&](uint64_t x) { accum += (x - mean) * (x - mean); });

  double stddev = std::sqrt(accum / data.size());

  std::cout << "Time Mean/Stddev: " << mean << "/" << stddev << '\n';
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
  std::vector<int64_t> results;

  for (int trial = 0; trial <= 10; ++trial) {
    mallocCount = 0;
    mallocSize = 0;
    auto start = clock::now();

    for (unsigned i = 0; i < loops; ++i) {
      flowMod(&channel, inPort, bufferId, dst, src, outPort);
      channel.clear();
    }

    auto end = clock::now();

    // Ignore trial 0.
    if (trial == 0)
        continue;

    milliseconds duration =
        std::chrono::duration_cast<milliseconds>(end - start);
    std::cout << "Loops=" << loops << " Trial " << std::setw(2) << trial
              << " Time: " << duration.count() << " ms  Malloc/Bytes: " << mallocCount << "/" << mallocSize << " (" << double(mallocCount) / loops << "/" << double(mallocSize) / loops << ")\n";
    results.push_back(duration.count());

  }

  logStats(results);

  return 0;
}
