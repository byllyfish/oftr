// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "./oftr_encode.h"

#include <fstream>
#include <iostream>

#include "ofp/yaml/decoder.h"
#include "ofp/yaml/encoder.h"

using namespace ofpx;
using ExitStatus = Encode::ExitStatus;

const char *const kNullYamlMessage = "---\nnull\n...\n";
const char *const kNullJsonMessage = "null\n";

int Encode::run(int argc, const char *const *argv) {
  parseCommandLineOptions(
      argc, argv,
      "Translate OpenFlow messages specified by YAML input files to binary\n");

  if (!validateCommandLineArguments()) {
    return static_cast<int>(ExitStatus::InvalidArguments);
  }

  // Set up output stream.
  using namespace llvm::sys;
  std::error_code err;
  output_.reset(new llvm::raw_fd_ostream{outputFile_, err, fs::F_None});
  if (err) {
    llvm::errs() << "Error: opening file for output " << outputFile_ << '\n';
    return static_cast<int>(ExitStatus::FileOpenFailed);
  }

  return static_cast<int>(encodeFiles());
}

bool Encode::validateCommandLineArguments() {
  // If there are no input files, add "-" to indicate stdin.
  if (inputFiles_.empty()) {
    inputFiles_.push_back("-");
  }

  // If output file is empty, use "-" to indicate stdout.
  if (outputFile_.empty()) {
    outputFile_ = "-";
  }

  // Set up `readMessage` function.
  if (jsonArray_) {
    readMessage_ = ofp::yaml::getjson;
  } else if (json_) {
    readMessage_ = ofp::yaml::getline;
  } else {
    readMessage_ = ofp::yaml::getyaml;
  }

  return true;
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
      llvm::errs() << "Error: can't open directory: " << filename << '\n';
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
    llvm::errs() << "Error: opening file " << filename << '\n';
  }

  return result;
}

ExitStatus Encode::encodeMessages(std::istream &input) {
  std::string text;
  int lineNum = 0;

  while (readMessage_(input, text, lineNum, lineNumber_)) {
    log_debug("readMessage line", lineNum, ':', text);

    ofp::yaml::Encoder encoder{text, !uncheckedMatch_, lineNum,
                               ofp::UInt8_narrow_cast(ofversion_.getValue())};

    auto err = encoder.error();
    if (!err.empty()) {
      // There was an error in converting the text to a binary message.
      if (roundtrip_ && !silent_) {
        // Send back an empty message to indicate `roundtrip` failure.
        *output_ << (json_ ? kNullJsonMessage : kNullYamlMessage);
      }
      if (!silentError_) {
        llvm::errs() << err << '\n';
      }
      if (!keepGoing_) {
        return ExitStatus::EncodeFailed;
      }

    } else if (roundtrip_) {
      // Translate binary message back to text.
      ofp::Message message{encoder.data(), encoder.size()};
      message.normalize();

      ofp::yaml::Decoder decoder{&message, json_};

      err = decoder.error();
      if (!err.empty()) {
        if (!silent_) {
          // Send back an empty message to indicate `roundtrip` failure.
          *output_ << (json_ ? kNullJsonMessage : kNullYamlMessage);
        }
        if (!silentError_) {
          llvm::errs() << err << '\n';
        }
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

    output_->flush();
  }

  if (!input.eof()) {
    // Premature I/O error; we're not at EOF.
    llvm::errs() << "Error: Error reading from file " << currentFilename_
                 << '\n';
    return ExitStatus::MessageReadFailed;
  }

  return ExitStatus::Success;
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
    output_->write(static_cast<const char *>(data), length);
  }
}
