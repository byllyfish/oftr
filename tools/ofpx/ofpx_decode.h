#ifndef OFPX_DECODE_H_
#define OFPX_DECODE_H_

#include "ofpx.h"
#include "ofp/message.h"

namespace ofpx {

class Decode : public Subprogram {
public:
    int run(int argc, char **argv) override;

private:
    enum class Verify {
      Valid,
      Invalid,
    };

    cl::opt<Verify> verify_{"verify", cl::desc("Verify messages."), 
        cl::values(
            clEnumValN(Verify::Valid, "valid", "expect valid messages only"),
            clEnumValN(Verify::Invalid, "invalid", "expect invalid messages only"),
            clEnumValEnd
        )};
    cl::opt<bool> quiet_{"quiet", cl::desc("Don't print output to stdout.")};
    cl::list<std::string> inputFiles_{cl::Positional, cl::desc("<Input files>"),
                                   cl::OneOrMore};

    std::string currentFilename_;

    int decodeFiles();
    int decodeFile(const std::string &filename);
    int decodeMessages(std::ifstream &input);
    int decodeOneMessage(const ofp::Message *message, const ofp::Message *originalMessage);
};

}  // namespace ofpx

#endif // OFPX_DECODE_H_
