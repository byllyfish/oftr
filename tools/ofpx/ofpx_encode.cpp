// Copyright 2014-present Bill Fisher. All rights reserved.

#include "./ofpx_encode.h"
#include <iostream>
#include <fstream>
#include "ofp/yaml/encoder.h"
#include "ofp/yaml/decoder.h"

using namespace ofpx;
using ExitStatus = Encode::ExitStatus;

int Encode::run(int argc, const char *const *argv) {
  cl::ParseCommandLineOptions(argc, argv);

  // If there are no input files, add "-" to indicate stdin.
  if (inputFiles_.empty()) {
    inputFiles_.push_back("-");
  }

  return static_cast<int>(encodeFiles());
}

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
    lineNumber_ = 0;
    result = encodeMessages(*input);
    currentFilename_ = "";
  } else {
    result = ExitStatus::FileOpenFailed;
    std::cerr << "Error: opening file " << filename << '\n';
  }

  return result;
}

ExitStatus Encode::encodeMessages(std::istream &input) {
  std::string text;
  int lineNum;

  while (readMessage(input, text, lineNum)) {
    ofp::yaml::Encoder encoder{text, !uncheckedMatch_, lineNum};

    auto err = encoder.error();
    if (!err.empty()) {
      // There was an error in converting the text to a binary message.
      std::cerr << err << '\n';
      if (!keepGoing_) {
        return ExitStatus::EncodeFailed;
      }

    } else if (roundtrip_) {
      // Translate binary message back to text.
      ofp::Message message{encoder.data(), encoder.size()};
      message.transmogrify();

      ofp::yaml::Decoder decoder{&message, json_};

      err = decoder.error();
      if (!err.empty()) {
        std::cerr << err << '\n';
        if (!keepGoing_) {
          return ExitStatus::RoundtripFailed;
        }
      } else if (!silent_) {
        std::cout << decoder.result();
      }

    } else if (!silent_) {
      output(encoder.data(), encoder.size());
    }
  }

  if (!input.eof()) {
    // Premature I/O error; we're not at EOF.
    std::cerr << "Error: Error reading from file " << currentFilename_ << '\n';
    return ExitStatus::MessageReadFailed;
  }

  return ExitStatus::Success;
}

static bool isEmptyOrWhitespaceOnly(std::string &s) {
  return std::find_if(s.begin(), s.end(),
                      [](char ch) { return !isspace(ch); }) == s.end();
}

bool Encode::readMessage(std::istream &input, std::string &msg, int &lineNum) {
  if (!input) {
    return false;
  }

  msg.clear();

  int msgLines = 0;
  while (input) {
    std::getline(input, lineBuf_);
    ++lineNumber_;

    if (json_) {
      if (isEmptyOrWhitespaceOnly(lineBuf_)) {
        // Don't return empty lines.
        continue;
      }
      msg = lineBuf_;
      return true;
    }

    assert(!json_);

    if (lineBuf_ == "---" || lineBuf_ == "...") {
      if (isEmptyOrWhitespaceOnly(msg)) {
        // Don't return empty messages.
        msgLines = 0;
        continue;
      }
      lineNum = lineNumber_ - msgLines - 1;
      return true;
    }
    msg += lineBuf_;
    msg += '\n';
    ++msgLines;
  }

  lineNum = lineNumber_ - msgLines - 1;

  return !isEmptyOrWhitespaceOnly(msg);
}

void Encode::output(const void *data, size_t length) {
  if (hex_) {
    // Output hex in rows of 4 blocks of 8 bytes each.
    const unsigned rowlen = 68;

    auto hex = ofp::RawDataToHex(data, length, ' ', 8);
    auto rows = hex.size() / rowlen;
    auto left = hex.size() % rowlen;

    for (auto i = 0U; i < rows; ++i) {
      std::cout << hex.substr(i * rowlen, rowlen) << '\n';
    }

    if (left) {
      std::cout << hex.substr(rows * rowlen, left) << '\n';
    }
    std::cout << '\n';

  } else {
    // Write binary message to stdout.
    std::cout.write(static_cast<const char *>(data), ofp::Signed_cast(length));
  }
}
