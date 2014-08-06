#ifndef OFPX_DECODE_H_
#define OFPX_DECODE_H_

#include "ofpx.h"

namespace ofpx {

// ofpx decode [--json|-j] [--silent|-s] [--invert-check|-v] [--keep-going|-k] [--verify-output|-V] [<Input files>]
//
// Decode binary OpenFlow messages in the input files and translate each
// message to human-readable YAML output. If there is an invalid message,
// stop and report an error.
// 
//   --json           Write compact JSON output instead of YAML.
//   --silent         Quiet mode; suppress normal output.
//   --invert-check   Expect invalid messages only.
//   --keep-going     Continue processing messages after errors.
//   --verify-output  Verify output by translating it back to binary.
// 
// Usage:
// 
// To decode a file of binary OpenFlow messages to YAML:
// 
//     ofpx decode "filename"
// 
// To decode a file of binary OpenFlow messages to JSON:
// 
//     ofpx decode --json "filename"
//     
// To process a file of invalid binary OpenFlow messages (e.g. fuzz test). This
// will report an error if any message is structurally valid as an OpenFlow 
// message:
// 
//     ofpx decode --invert-check "filename"
//     
// If no input files are specified, use standard input (stdin). A single hyphen
// also represents stdin.

class Decode : public Subprogram {
public:
    enum class ExitStatus {
        Success = 0,
        DecodeFailed = MinExitStatus,
        DecodeSucceeded,
        VerifyOutputFailed,
        FileOpenFailed,
        MessageReadFailed
    };

    int run(int argc, char **argv) override;

private:
    std::string currentFilename_;

    ExitStatus decodeFiles();
    ExitStatus decodeFile(const std::string &filename);
    ExitStatus decodeMessages(std::istream &input);
    ExitStatus checkError(std::istream &input, std::streamsize readLen, bool header);
    ExitStatus decodeOneMessage(const ofp::Message *message, const ofp::Message *originalMessage);

    // --- Command-line Arguments (Order is important here.) ---
    cl::opt<bool> json_{"json", cl::desc("Write compact JSON output instead of YAML")};
    cl::opt<bool> silent_{"silent", cl::desc("Quiet mode; suppress normal output")};
    cl::opt<bool> invertCheck_{"invert-check", cl::desc("Expect invalid messages only")};
    cl::opt<bool> keepGoing_{"keep-going", cl::desc("Continue processing messages after errors")};
    cl::opt<bool> verifyOutput_{"verify-output", cl::desc("Verify output by translating it back to binary")};
    cl::list<std::string> inputFiles_{cl::Positional, cl::desc("<Input files>")};

    // --- Argument Aliases (May be grouped into one argument) ---
    cl::alias jAlias_{"j", cl::desc("Alias for -json"), cl::aliasopt(json_), cl::Grouping };
    cl::alias sAlias_{"s", cl::desc("Alias for -silent"), cl::aliasopt(silent_), cl::Grouping};
    cl::alias vAlias_{"v", cl::desc("Alias for -invert-check"), cl::aliasopt(invertCheck_), cl::Grouping};
    cl::alias kAlias_{"k", cl::desc("Alias for -keep-going"), cl::aliasopt(keepGoing_), cl::Grouping};
    cl::alias VAlias_{"V", cl::desc("Alias for -verify-output"), cl::aliasopt(verifyOutput_), cl::Grouping};
};

}  // namespace ofpx

#endif // OFPX_DECODE_H_
