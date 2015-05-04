// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef TOOLS_OFPX_OFPX_ENCODE_H_
#define TOOLS_OFPX_OFPX_ENCODE_H_

#include "./ofpx.h"

namespace ofpx {

// ofpx encode [options] [<Input files>]
//
// Encode OpenFlow messages to binary as specified in YAML input files. If there
// is a syntax error in the YAML input, stop and report an error.
//
//   --hex, -x               Output hexadecimal rather than binary
//   --silent, -S            Quiet mode; suppress normal output
//   --keep-going, -k        Continue processing messages after errors
//   --unchecked-match, -M   Do not check items in match fields
//   --roundtrip, -R         Roundtrip encoded binary message back to YAML
//   --json, -j              Json input is separated by linefeeds
//   --ofversion=0           OpenFlow version to use when unspecified
//   --output=<file>         Write output to specified file instead of stdout.
//
// Usage:
//
// To translate a text file into binary OpenFlow messages:
//
//   ofpx encode "filename"
//
// To translate a text file into binary OpenFlow messages, while ignoring
// sets of match fields that violate prerequisites:
//
//   ofpx encode --unchecked-match "filename"
//
// To translate a text file of compact, single line JSON objects separated by
// linefeeds:
//
//   ofpx encode --json "filename"
//
// If no input files are specified, use standard input (stdin). A single hyphen
// also represents stdin.
//
// Each binary OpenFlow message will use the version specified by the textual
// object. Normally, it is an error if no version is specified. However, you
// can specify a default OpenFlow version using the --ofversion option.

OFP_BEGIN_IGNORE_PADDING

class Encode : public Subprogram {
 public:
  enum class ExitStatus {
    Success = 0,
    FileOpenFailed = FileOpenFailedExitStatus,
    EncodeFailed = MinExitStatus,
    RoundtripFailed,
    MessageReadFailed
  };

  int run(int argc, const char *const *argv) override;

 private:
  std::string currentFilename_;
  std::ostream *output_ = nullptr;
  std::string lineBuf_;
  int lineNumber_ = 0;

  ExitStatus encodeFiles();
  ExitStatus encodeFile(const std::string &filename);
  ExitStatus encodeMessages(std::istream &input);
  bool readMessage(std::istream &input, std::string &msg, int &lineNum);
  void output(const void *data, size_t length);

  // --- Command-line Arguments ---
  cl::opt<bool> hex_{"hex", cl::desc("Output hexadecimal rather than binary")};
  cl::opt<bool> silent_{"silent",
                        cl::desc("Quiet mode; suppress normal output")};
  cl::opt<bool> keepGoing_{
      "keep-going", cl::desc("Continue processing messages after errors")};
  cl::opt<bool> uncheckedMatch_{"unchecked-match",
                                cl::desc("Do not check items in match fields")};
  cl::opt<bool> roundtrip_{
      "roundtrip", cl::desc("Roundtrip encoded binary message back to YAML")};
  cl::opt<bool> json_{"json", cl::desc("Json input is separated by linefeeds")};
  cl::opt<unsigned> ofversion_{
      "ofversion", cl::desc("OpenFlow version to use when unspecified"),
      cl::ValueRequired};
  cl::opt<std::string> outputFile_{
      "output", cl::desc("Write output to specified file instead of stdout"),
      cl::ValueRequired};
  cl::list<std::string> inputFiles_{cl::Positional, cl::desc("<Input files>")};

  // --- Argument Aliases (May be grouped into one argument) ---
  cl::alias xAlias_{"x", cl::desc("Alias for -hex"), cl::aliasopt(hex_),
                    cl::Grouping};
  cl::alias sAlias_{"s", cl::desc("Alias for -silent"), cl::aliasopt(silent_),
                    cl::Grouping};
  cl::alias kAlias_{"k", cl::desc("Alias for -keep-going"),
                    cl::aliasopt(keepGoing_), cl::Grouping};
  cl::alias MAlias_{"M", cl::desc("Alias for -unchecked-match"),
                    cl::aliasopt(uncheckedMatch_), cl::Grouping};
  cl::alias RAlias_{"R", cl::desc("Alias for -roundtrip"),
                    cl::aliasopt(roundtrip_), cl::Grouping};
  cl::alias jAlias_{"j", cl::desc("Alias for -json"), cl::aliasopt(json_),
                    cl::Grouping};
  cl::alias oAlias_{"o", cl::desc("Alias for -output"),
                    cl::aliasopt(outputFile_)};
};

OFP_END_IGNORE_PADDING

}  // namespace ofpx

#endif  // TOOLS_OFPX_OFPX_ENCODE_H_
