// Copyright 2014-present Bill Fisher. All rights reserved.

#include "llvm/Support/CommandLine.h"
#include "ofpx_decode.h"
#include "ofp/yaml/decoder.h"
#include "ofp/yaml/encoder.h"
#include <iostream>
#include <fstream>

using namespace ofpx;
using ofp::UInt8;
using ExitStatus = Decode::ExitStatus;

// Compare two buffers and return offset of the byte that differs. If buffers
// are identical, return `size`.
//
static size_t findDiffOffset(const UInt8 *lhs, const UInt8 *rhs, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    if (lhs[i] != rhs[i]) return i;
  }
  return size;
}

//-------//
// r u n //
//-------//

int Decode::run(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv);

  // If there are no input files, add "-" to indicate stdin.
  if (inputFiles_.empty()) {
    inputFiles_.push_back("-");
  }

  return static_cast<int>(decodeFiles());
}

//-----------------------//
// d e c o d e F i l e s //
//-----------------------//

ExitStatus Decode::decodeFiles() {
  const std::vector<std::string> &files = inputFiles_;

  for (const std::string &filename : files) {
    ExitStatus result = decodeFile(filename);
    if (result != ExitStatus::Success && !keepGoing_) {
      return result;
    }
  }

  return ExitStatus::Success;
}

//---------------------//
// d e c o d e F i l e //
//---------------------//

ExitStatus Decode::decodeFile(const std::string &filename) {
  ExitStatus result;

  std::istream *input = nullptr;
  std::ifstream file;
  if (filename != "-") {
    file.open(filename, std::ifstream::binary);
    input = &file;
  } else {
    input = &std::cin;
  }

  if (*input) {
    // Store current filename in instance variable for use in error messages.
    currentFilename_ = filename;
    result = decodeMessages(*input);
    currentFilename_ = "";
  } else {
    result = ExitStatus::FileOpenFailed;
    std::cerr << "Error: opening file " << filename << '\n';
  }

  return result;
}

//-----------------------------//
// d e c o d e M e s s a g e s //
//-----------------------------//

ExitStatus Decode::decodeMessages(std::istream &input) {
  // Create message buffers.
  ofp::Message message{nullptr};
  ofp::Message originalMessage{nullptr};

  while (input) {
    // Read the message header.
    char *msg = (char *)message.mutableData(sizeof(ofp::Header));

    input.read(msg, sizeof(ofp::Header));
    if (!input) {
      return checkError(input, sizeof(ofp::Header), true);
    }

    // Check header length in message. If the header says the length is less
    // than 8 bytes, the entire message *must* still be 8 bytes in length.
    size_t msgLen = message.header()->length();
    if (msgLen < sizeof(ofp::Header)) {
      msgLen = sizeof(ofp::Header);
    }

    // Read the message body.
    msg = (char *)message.mutableData(msgLen);
    std::streamsize bodyLen = ofp::Signed_cast(msgLen - sizeof(ofp::Header));

    input.read(msg + sizeof(ofp::Header), bodyLen);
    if (!input) {
      return checkError(input, bodyLen, false);
    }

    // Save a copy of the original message binary before we transmogrify it
    // for parsing. After we decode the message, we'll re-encode it and
    // compare it to this original.

    originalMessage.assign(message);
    message.transmogrify();

    ExitStatus result = decodeOneMessage(&message, &originalMessage);
    if (result != ExitStatus::Success && !keepGoing_) {
      return result;
    }
  }

  return ExitStatus::Success;
}

//---------------------//
// c h e c k E r r o r //
//---------------------//

ExitStatus Decode::checkError(std::istream &input, std::streamsize readLen,
                              bool header) {
  assert(!input);

  if (!input.eof()) {
    // Premature I/O error; we're not at EOF.
    std::cerr << "Error: Error reading from file " << currentFilename_ << '\n';
    return ExitStatus::MessageReadFailed;
  } else if (input.gcount() != readLen && !(header && input.gcount() == 0)) {
    // EOF and insufficient input remaining. N.B. Zero bytes of header read at
    // EOF is a normal exit condition.
    const char *what = header ? "header" : "body";
    std::cerr << "Error: Only " << input.gcount() << " bytes read of " << what
              << '\n';
    return ExitStatus::MessageReadFailed;
  } else {
    // EOF and everything is good.
    return ExitStatus::Success;
  }
}

//---------------------------------//
// d e c o d e O n e M e s s a g e //
//---------------------------------//

ExitStatus Decode::decodeOneMessage(const ofp::Message *message,
                                    const ofp::Message *originalMessage) {
  ofp::yaml::Decoder decoder{message, json_};

  if (!decoder.error().empty()) {
    // An error occurred in decoding the message.

    if (invertCheck_) {
      // We're expecting invalid data -- report no error and continue.
      return ExitStatus::Success;
    }

    std::cerr << "Filename: " << currentFilename_ << '\n';
    std::cerr << "Error: Decode failed: " << decoder.error() << '\n';
    std::cerr << *originalMessage << '\n';
    return ExitStatus::DecodeFailed;
  }

  if (invertCheck_) {
    // There was no problem decoding the message, but we are expecting the data
    // to be invalid. Report this as an error.

    std::cerr << "Filename: " << currentFilename_ << '\n';
    std::cerr
        << "Error: Decode succeeded when --invert-check flag is specified.\n";
    std::cerr << *originalMessage << '\n';
    return ExitStatus::DecodeSucceeded;
  }

  if (!silent_) {
    std::cout << decoder.result();
    if (json_) {
      std::cout << '\n';
    }
  }

  if (verifyOutput_) {
    // Double-check the result by re-encoding the YAML message. We should obtain
    // the original message contents. If there is a difference, report the
    // error.

    ofp::yaml::Encoder encoder{decoder.result(), false};

    if (!encoder.error().empty()) {
      std::cerr << "Error: Decode succeeded but encode failed: "
                << encoder.error() << '\n';
      return ExitStatus::VerifyOutputFailed;
    }

    if (encoder.size() != originalMessage->size()) {
      std::cerr << "Error: Encode yielded different size data: "
                << encoder.size() << " vs. " << originalMessage->size() << '\n'
                << ofp::RawDataToHex(encoder.data(), encoder.size()) << '\n'
                << ofp::RawDataToHex(originalMessage->data(),
                                     originalMessage->size()) << '\n';
      return ExitStatus::VerifyOutputFailed;

    } else if (std::memcmp(originalMessage->data(), encoder.data(),
                           encoder.size()) != 0) {
      size_t diffOffset = findDiffOffset(originalMessage->data(),
                                         encoder.data(), encoder.size());
      std::cerr << "Error: Encode yielded different data at byte offset "
                << diffOffset << ":\n"
                << ofp::RawDataToHex(encoder.data(), encoder.size()) << '\n'
                << ofp::RawDataToHex(originalMessage->data(),
                                     originalMessage->size()) << '\n';
      return ExitStatus::VerifyOutputFailed;
    }
  }

  return ExitStatus::Success;
}
