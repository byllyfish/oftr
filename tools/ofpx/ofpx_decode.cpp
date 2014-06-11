#include "llvm/Support/CommandLine.h"
#include "ofpx_decode.h"
#include "ofp/yaml/decoder.h"
#include "ofp/yaml/encoder.h"
#include <iostream>
#include <fstream>

using namespace ofpx;

//-------//
// r u n //
//-------//

int Decode::run(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv);
  return decodeFiles();
}

//-----------------------//
// d e c o d e F i l e s //
//-----------------------//

int Decode::decodeFiles() {
  const std::vector<std::string> &files = inputFiles_;

  for (const std::string &filename : files) {
    int result = decodeFile(filename);
    if (result) {
      return result;
    }
  }

  return 0;
}


//---------------------//
// d e c o d e F i l e //
//---------------------//

int Decode::decodeFile(const std::string &filename) {
  std::ifstream input{filename, std::ifstream::binary};
  int result = -1;

  if (input) {
    currentFilename_ = filename;
    result = decodeMessages(input);
    input.close();
    currentFilename_ = "";
  } else {
    std::cerr << "Error: opening file " << filename << '\n';
  }

  return result;
}


//-----------------------------//
// d e c o d e M e s s a g e s //
//-----------------------------//

int Decode::decodeMessages(std::ifstream &input) {
  ofp::Message message{nullptr};
  ofp::Message originalMessage{nullptr};
  size_t offset = 0;

  while (input) {
    // Read the message header.
    char *msg = (char *)message.mutableData(sizeof(ofp::Header));
    input.read(msg, sizeof(ofp::Header));

    if (input) {
      // Read the rest of the message.

      assert(input.gcount() == sizeof(ofp::Header));
      size_t msgLen = message.header()->length();
      if (msgLen < sizeof(ofp::Header)) {
        std::cerr << "Error: Message length " << msgLen
                  << " bytes is too short: offset=" << offset << '\n' << ofp::RawDataToHex(msg, sizeof(ofp::Header)) << '\n';
        offset += sizeof(ofp::Header);
        continue;
      }

      msg = (char *)message.mutableData(msgLen);
      input.read(msg + sizeof(ofp::Header),
                 ofp::Signed_cast(msgLen - sizeof(ofp::Header)));

      if (input) {
        // Save a copy of the original message binary before we transmogrify it
        // for reading. After we decode the message, we'll re-encode it and 
        // compare it to this original.

        originalMessage.assign(message);
        message.transmogrify();

        int result = decodeOneMessage(&message, &originalMessage);
        if (result) {
          return result;
        }

        offset += msgLen;

      } else if (!input.eof()) {
        // There was an error reading the message body.
        std::cerr << "Error: Only " << input.gcount() << " bytes read of body."
                  << '\n';
        return 1;
      }

    } else if (!input.eof()) {
      // There was an error reading the message header.
      std::cerr << "Error: Only " << input.gcount() << " bytes read of header."
                << '\n' << input.rdstate();
      return 1;
    }
  }

  return 0;
}


//---------------------------------//
// d e c o d e O n e M e s s a g e //
//---------------------------------//

int Decode::decodeOneMessage(const ofp::Message *message, const ofp::Message *originalMessage) {
  ofp::yaml::Decoder decoder{message};

  if (!decoder.error().empty()) {
    // An error occurred in decoding the message.
    
    if (verify_ == Verify::Valid) {
      // If the verify flag indicates that we expect the data to be valid, we 
      // report the error.

      std::cerr << "Filename: " << currentFilename_ << '\n';
      std::cerr << "Error: Decode failed: " << decoder.error() << '\n';
      std::cerr << *message << '\n';
      return 128;

    } else {
      assert(verify_ == Verify::Invalid);

      // Report no error and continue.
      return 0;
    }
  }

  if (verify_ == Verify::Invalid) {
    // There was no problem decoding the message, but we are expecting the data
    // to be invalid. Report this as an error.
    
    std::cerr << "Filename: " << currentFilename_ << '\n';
    std::cerr << "Error: Decode succeeded when --verify=invalid flag is specified.\n";
    //std::cerr << *message << '\n';
    //return 0;
  }

  if (!quiet_) {
    std::cout << decoder.result();
  }

  // Now double-check the result by re-encoding the message. We should obtain 
  // the original message contents.

  ofp::yaml::Encoder encoder{decoder.result(), false};

  if (!encoder.error().empty()) {
    std::cerr << "Error: Decode succeeded but encode failed: " << encoder.error() << '\n';
    //return 129;
  }

  if (encoder.size() != originalMessage->size()) {
    std::cerr << "Error: Encode yielded different size data: " << encoder.size() << " vs. " << originalMessage->size() << '\n' << ofp::RawDataToHex(encoder.data(), encoder.size()) << '\n' << ofp::RawDataToHex(originalMessage->data(), originalMessage->size()) << '\n';
    //return 130;
  } else if (std::memcmp(originalMessage->data(), encoder.data(), encoder.size()) != 0) {
    std::cerr << "Error: Encode yielded different data:\n" << ofp::RawDataToHex(encoder.data(), encoder.size()) << '\n' << ofp::RawDataToHex(originalMessage->data(), originalMessage->size()) << '\n';
    //return 131;
  }

  return 0;
}




