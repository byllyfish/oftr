//#include "llvm/Support/CommandLine.h"
#include "ofpx_encode.h"
#include "ofp/yaml/encoder.h"
#include <iostream>
#include <fstream>

using namespace ofpx;
using ExitStatus = Encode::ExitStatus;


//-------//
// r u n //
//-------//

int Encode::run(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv);

  // If there are no input files, add "-" to indicate stdin.
  if (inputFiles_.empty()) {
    inputFiles_.push_back("-");
  }

  return static_cast<int>(encodeFiles());
}


//-----------------------//
// e n c o d e F i l e s //
//-----------------------//

ExitStatus Encode::encodeFiles() {
  const std::vector<std::string> &files = inputFiles_;

  for (const std::string &filename : files) {
    ExitStatus result = encodeFile(filename);
    if (result != ExitStatus::Success && !keepGoing_) {
      return result;
    }
  }

  return ExitStatus::Success;
}


//---------------------//
// e n c o d e F i l e //
//---------------------//

ExitStatus Encode::encodeFile(const std::string &filename) {
  ExitStatus result;

  std::istream *input = nullptr;
  std::ifstream file;
  if (filename != "-") {
    file.open(filename);
    input = &file;
  } else {
    input = &std::cin;
  }

  if (*input) {
    // Store current filename in instance variable for use in error messages.
    currentFilename_ = filename;
    result = encodeMessages(*input);
    currentFilename_ = "";
  } else {
    result = ExitStatus::FileOpenFailed;
    std::cerr << "Error: opening file " << filename << '\n';
  }

  return result;
}


//-----------------------------//
// e n c o d e M e s s a g e s //
//-----------------------------//

ExitStatus Encode::encodeMessages(std::istream &input) {
  std::string text;

  while (readMessage(input, text)) {
    ofp::yaml::Encoder encoder{text, !uncheckedMatch_};

    auto err = encoder.error();
    if (!err.empty()) {
        std::cerr << err << '\n';
        if (!keepGoing_) {
          return ExitStatus::EncodeFailed;
        }

    } else if (!silent_) {
        std::cout.write(reinterpret_cast<const char *>(encoder.data()), ofp::Signed_cast(encoder.size()));
    }
  }

  if (!input.eof()) {
    // Premature I/O error; we're not at EOF.
    std::cerr << "Error: Error reading from file " << currentFilename_ << '\n';
    return ExitStatus::MessageReadFailed;
  }

  return ExitStatus::Success;
}


//-----------------------------------------------//
// i s E m p t y O r W h i t e s p a c e O n l y //
//-----------------------------------------------//

static bool isEmptyOrWhitespaceOnly(std::string &s) {
  return std::find_if(s.begin(), s.end(), [](char ch) { return !isspace(ch); }) == s.end();
}


//-----------------------//
// r e a d M e s s a g e //
//-----------------------//

bool Encode::readMessage(std::istream &input, std::string &msg) {
    if (!input) {
        return false;
    }

    msg.clear();

    while (input) {
        std::getline(input, lineBuf_);
        if (lineBuf_ == "---" || lineBuf_ == "...") {
            if (isEmptyOrWhitespaceOnly(msg)) {
              // Don't return empty messages.
              continue;
            }
            return true;
        }
        msg += lineBuf_;
        msg += '\n';
    }

    return !isEmptyOrWhitespaceOnly(msg);
}
