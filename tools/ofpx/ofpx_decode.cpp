// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofpx_decode.h"
#include <iostream>
#include <fstream>
#include "ofp/yaml/decoder.h"
#include "ofp/yaml/encoder.h"

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

int Decode::run(int argc, const char *const *argv) {
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
  std::istream *input = nullptr;
  std::ifstream file;
  if (filename != "-") {
    file.open(filename, std::ifstream::binary);
    input = &file;
  } else {
    input = &std::cin;
  }

  if (!*input) {
    std::cerr << "Error: opening file " << filename << '\n';
    return ExitStatus::FileOpenFailed;
  }

  ExitStatus result;

  if (useFindx_) {
    // There is no .findx file for stdin.
    if (filename == "-") {
      std::cerr << "Error: stdin has no index file";
      return ExitStatus::FileOpenFailed;
    }

    std::ifstream index(filename + ".findx");
    if (!index) {
      std::cerr << "Error: opening index file " << filename << ".findx\n";
      return ExitStatus::FileOpenFailed;
    }

    currentFilename_ = filename;
    result = decodeMessagesWithIndex(*input, index);

  } else {
    // Decode messages from file normally -- no index file.
    currentFilename_ = filename;
    result = decodeMessages(*input);
  }

  currentFilename_ = "";

  return result;
}

//-----------------------------//
// d e c o d e M e s s a g e s //
//-----------------------------//

ExitStatus Decode::decodeMessages(std::istream &input) {
  // Create message buffers.
  ofp::Message message{nullptr};
  ofp::Message originalMessage{nullptr};
  ofp::Timestamp timestamp;

  while (input) {
    // Read the message header.
    char *msg =
        reinterpret_cast<char *>(message.mutableData(sizeof(ofp::Header)));

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
    msg = reinterpret_cast<char *>(message.mutableData(msgLen));
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

    ExitStatus result = decodeOneMessage(&message, &originalMessage, timestamp);
    if (result != ExitStatus::Success && !keepGoing_) {
      return result;
    }
  }

  return ExitStatus::Success;
}

//-----------------------------------------------//
// d e c o d e M e s s a g e s W i t h I n d e x //
//-----------------------------------------------//

ExitStatus Decode::decodeMessagesWithIndex(std::istream &input, std::istream &index) {
  // Create message buffers.
  ofp::Message message{nullptr};
  ofp::Message originalMessage{nullptr};
  ofp::Message buffer{nullptr};
  size_t expectedPos = 0;

  buffer.shrink(0);
  assert(buffer.size() == 0);

  // Read lines from index file.
  for (std::string line; std::getline(index, line);) {
    ofp::Timestamp timestamp;
    size_t pos;
    size_t length;

    // Parse line to obtain position, timestamp and length.
    if (!parseIndexLine(line, &pos, &timestamp, &length)) {
      std::cerr << "Error in parsing index: " << line << " file=" << currentFilename_ << '\n';
      return ExitStatus::IndexReadFailed;
    }

    // Check for gaps in the stream.
    if (pos < expectedPos) {
      std::cerr << "Error in index; data offset is backwards: " << line << " file=" << currentFilename_ << '\n';
      continue;
      
    } else if (pos > expectedPos) {
      std::cerr << "Gap in stream (" << pos - expectedPos << " bytes) file=" << currentFilename_ << '\n';
      auto jump = ofp::Signed_cast(pos - expectedPos);
      if (!input.ignore(jump)) {
        return checkError(input, jump, false);
      }
    }

    // Set up next expectedPos.
    expectedPos = pos + length;

    size_t offset = buffer.size();
    char *buf = reinterpret_cast<char *>(buffer.mutableData(offset + length));
    assert(buffer.size() == offset + length);

    // Read length bytes from input into buffer.
    input.read(buf + offset, ofp::Signed_cast(length));
    if (!input) {
      return checkError(input, ofp::Signed_cast(length), false);
    }

    // Log when messages do not align to packet 'boundaries'.
    if (buffer.size() < sizeof(ofp::Header)) {
      std::cerr << "Header fragmented (" << buffer.size() << " bytes) " << currentFilename_ << '\n';
    } else if (buffer.header()->length() > buffer.size()) {
      std::cerr << "Message fragmented (" << length << " of " << buffer.header()->length() << " bytes) in " << currentFilename_ << '\n';
    }

    // Decode complete messages and assign them the last read timestamp.
    while (buffer.size() >= sizeof(ofp::Header) && buffer.header()->length() <= buffer.size()) {
      message.setTime(timestamp);
      message.setData(buffer.data(), buffer.header()->length());
      buffer.removeFront(buffer.header()->length());

      if (message.size() < sizeof(ofp::Header)) {
        // If message size is less than 8 bytes, report an error.
        std::cerr << "Filename: " << currentFilename_ << ": " << line << '\n';
        std::cerr << "Error: Invalid message header length: " << message.size() << " bytes\n";
        return ExitStatus::DecodeFailed;
      }

      // Save a copy of the original message binary before we transmogrify it
      // for parsing.
      originalMessage.assign(message);
      message.transmogrify();

      ExitStatus result = decodeOneMessage(&message, &originalMessage, timestamp);
      if (result != ExitStatus::Success && !keepGoing_) {
        return result;
      }    
    }
  }

  // Check that we reached end of index file without error.
  if (!index.eof()) {
    std::cerr << "Error: Error reading from index file " << currentFilename_ << ".findx\n";
    return ExitStatus::IndexReadFailed;
  }

  // We should not be able to read any more data from input. If we can, the 
  // index file is not synced with the input file.
  char ch;
  if (input.get(ch)) {
    std::cerr << "Error: Unexpected data in file " << currentFilename_ << '\n';
    return ExitStatus::MessageReadFailed;
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
                                    const ofp::Message *originalMessage, const ofp::Timestamp &timestamp) {
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

//-----------------------------//
// p a r s e I n d e x L i n e //
//-----------------------------//

bool Decode::parseIndexLine(const llvm::StringRef &line, size_t *pos, ofp::Timestamp *timestamp, size_t *length) {
  // Each line has the format:
  // 
  //     pos|timestamp|length
  
  auto posEnd = line.find_first_of('|');
  if (posEnd == llvm::StringRef::npos) 
    return false;

  auto timestampEnd = line.find_first_of('|', posEnd + 1);
  if (timestampEnd == llvm::StringRef::npos) 
    return false;

  assert(timestampEnd >= posEnd + 1);
  
  auto posStr = line.substr(0, posEnd);
  auto timestampStr = line.substr(posEnd + 1, timestampEnd - posEnd - 1);
  auto lengthStr = line.substr(timestampEnd + 1);

  if (posStr.getAsInteger(10, *pos))
    return false;

  if (lengthStr.getAsInteger(10, *length)) 
    return false;

  return timestamp->parse(timestampStr);
}
