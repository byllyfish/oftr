#ifndef OFPX_ENCODE_H_
#define OFPX_ENCODE_H_

#include "ofpx.h"

namespace ofpx {

// ofpx encode [<Input files>]
//
// Encode OpenFlow messages as specified in YAML input files. If there is a
// syntax error in the YAML input, stop and report an error.
// 

class Encode : public Subprogram {
public:
    enum class ExitStatus {
        Success = 0,
        EncodeFailed = MinExitStatus,
        FileOpenFailed,
        MessageReadFailed
    };

    int run(int argc, char **argv) override;

private:
    std::string currentFilename_;
    std::string lineBuf_;

    ExitStatus encodeFiles();
    ExitStatus encodeFile(const std::string &filename);
    ExitStatus encodeMessages(std::istream &input);
    bool readMessage(std::istream &input, std::string &msg);

    // --- Command-line Arguments ---
    cl::opt<bool> silent_{"silent", cl::desc("Quiet mode; suppress normal output")};
    cl::opt<bool> keepGoing_{"keep-going", cl::desc("Continue processing messages after errors")};
    cl::opt<bool> uncheckedMatch_{"unchecked-match", cl::desc("Do not check items in match fields")};
    cl::list<std::string> inputFiles_{cl::Positional, cl::desc("<Input files>")};

    // --- Argument Aliases (May be grouped into one argument) ---
    cl::alias sAlias_{"s", cl::desc("Alias for -silent"), cl::aliasopt(silent_), cl::Grouping};
    cl::alias kAlias_{"k", cl::desc("Alias for -keep-going"), cl::aliasopt(keepGoing_), cl::Grouping};
    cl::alias MAlias_{"M", cl::desc("Alias for -unchecked-match"), cl::aliasopt(uncheckedMatch_), cl::Grouping};
};

}  // namespace ofpx

#endif // OFPX_ENCODE_H_
