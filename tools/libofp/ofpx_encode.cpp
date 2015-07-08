// Copyright 2014-present Bill Fisher. All rights reserved.

#include "./ofpx_encode.h"
#include <iostream>
#include <fstream>
#include "ofp/yaml/encoder.h"
#include "ofp/yaml/decoder.h"

using namespace ofpx;
using ExitStatus = Encode::ExitStatus;

static const char *kNullYamlMessage = "---\nnull\n...\n";
static const char *kNullJsonMessage = "null\n";

int Encode::run(int argc, const char *const *argv) {
  parseCommandLineOptions(
      argc, argv,
      "Translate OpenFlow messages specified by YAML input files to binary\n");

  // If there are no input files, add "-" to indicate stdin.
  if (inputFiles_.empty()) {
    inputFiles_.push_back("-");
  }

  // Set up output stream.
  std::ofstream outStream;
  if (outputFile_.empty()) {
    output_ = &std::cout;
  } else {
    outStream.open(outputFile_);
    if (!outStream) {
      std::cerr << "Error: opening file for output " << outputFile_ << '\n';
      return static_cast<int>(ExitStatus::FileOpenFailed);
    }
    output_ = &outStream;
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
    // Check if filename is a directory.
    if (llvm::sys::fs::is_directory(filename)) {
      std::cerr << "Error: can't open directory: " << filename << '\n';
      return ExitStatus::FileOpenFailed;
    }
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
    ofp::log::debug("readMessage returned", text);

    ofp::yaml::Encoder encoder{text, !uncheckedMatch_, lineNum,
                               ofp::UInt8_narrow_cast(ofversion_.getValue())};

    auto err = encoder.error();
    if (!err.empty()) {
      // There was an error in converting the text to a binary message.
      if (roundtrip_ && !silent_) {
        // Send back an empty message to indicate `roundtrip` failure.
        *output_ << (json_ ? kNullJsonMessage : kNullYamlMessage);
      }      
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
        if (!silent_) {
          // Send back an empty message to indicate `roundtrip` failure.
          *output_ << (json_ ? kNullJsonMessage : kNullYamlMessage);
        }
        std::cerr << err << '\n';
        if (!keepGoing_) {
          return ExitStatus::RoundtripFailed;
        }
      } else if (!silent_) {
        *output_ << decoder.result();
        if (json_)
          *output_ << '\n';
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
  while (std::getline(input, lineBuf_)) {
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
      *output_ << hex.substr(i * rowlen, rowlen) << '\n';
    }

    if (left) {
      *output_ << hex.substr(rows * rowlen, left) << '\n';
    }
    *output_ << '\n';

  } else {
    // Write binary message to stdout.
    output_->write(static_cast<const char *>(data), ofp::Signed_cast(length));
  }
}
