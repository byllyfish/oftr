// The following two definitions are required by llvm/Support/DataTypes.h
#define __STDC_LIMIT_MACROS 1
#define __STDC_CONSTANT_MACROS 1

#include "llvm/Support/CommandLine.h"
#include "ofpx.h"
#include "ofp/yaml/decoder.h"
#include "ofp/yaml/encoder.h"
#include <iostream>
#include <fstream>

//-------------------------------------//
// d e c o d e _ o n e _ m e s s a g e //
//-------------------------------------//

static int decode_one_message(const ofp::Message *message) {
  ofp::yaml::Decoder decoder{message};

  if (!decoder.error().empty()) {
    std::cerr << "Error: Decode failed: " << decoder.error() << '\n';
    return 128;
  }

  std::cout << decoder.result();

  // Now double-check the result by re-encoding the message. We should obtain 
  // the original message contents.

  ofp::yaml::Encoder encoder{decoder.result()};

  if (!encoder.error().empty()) {
    std::cerr << "Error: Decode succeeded but encode failed: " << encoder.error() << '\n';
    return 129;
  }

  if (encoder.size() != message->size()) {
    std::cerr << "Error: Encode yielded different size data: " << encoder.size() << " vs. " << message->size() << '\n' << ofp::RawDataToHex(encoder.data(), encoder.size()) << '\n';
    return 130;
  }

  if (std::memcmp(message->data(), encoder.data(), encoder.size()) != 0) {
    std::cerr << "Error: Encode yielded different data:\n" << ofp::RawDataToHex(encoder.data(), encoder.size()) << '\n' << ofp::RawDataToHex(message->data(), message->size()) << '\n';
    //return 131;
  }

  return 0;
}

//-------------------------------//
// d e c o d e _ m e s s a g e s //
//-------------------------------//

static int decode_messages(std::ifstream &input) {
  ofp::Message message{nullptr};

  while (input) {
    char *msg = (char *)message.mutableData(sizeof(ofp::Header));

    input.read(msg, sizeof(ofp::Header));
    if (input) {
      // Read the rest of the message.

      size_t msgLen = message.header()->length();
      if (msgLen < sizeof(ofp::Header)) {
        std::cerr << "Error: Message length " << msgLen
                  << " bytes is too short." << '\n';
        return 2;
      }

      msg = (char *)message.mutableData(msgLen);
      input.read(msg + sizeof(ofp::Header),
                 ofp::Signed_cast(msgLen - sizeof(ofp::Header)));

      if (input) {
        message.transmogrify();

        int result = decode_one_message(&message);
        if (result) {
          return result;
        }

      } else if (!input.eof()) {
        // There was an error reading the message body.
        std::cerr << "Error: Only " << input.gcount() << " bytes read of body."
                  << '\n' << input.rdstate();
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

//-----------------------//
// d e c o d e _ f i l e //
//-----------------------//

static int decode_file(const std::string &filename) {
  std::ifstream input{filename, std::ifstream::binary};
  int result = -1;

  if (input) {
    result = decode_messages(input);
    input.close();
  } else {
    std::cerr << "Error: opening file " << filename << '\n';
  }

  return result;
}

//-------------------------//
// d e c o d e _ f i l e s //
//-------------------------//

static int decode_files(const std::vector<std::string> &files) {
  for (const std::string &filename : files) {
    int result = decode_file(filename);
    if (result) {
      return result;
    }
  }

  return 0;
}

//-------------------//
// o f p x _ p i n g //
//-------------------//

int ofpx_decode(int argc, char **argv) {
  using namespace llvm;

  cl::list<std::string> inputFiles(cl::Positional, cl::desc("<Input files>"),
                                   cl::OneOrMore);

  cl::ParseCommandLineOptions(argc, argv);

  return decode_files(inputFiles);
}
