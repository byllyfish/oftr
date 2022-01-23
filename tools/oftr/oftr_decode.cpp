// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "./oftr_decode.h"

#include <fnmatch.h>  // for fnmatch()

#include <fstream>
#include <iostream>

#include "./oftr_util.h"
#include "llvm/Support/Path.h"
#include "ofp/log.h"
#include "ofp/yaml/decoder.h"
#include "ofp/yaml/encoder.h"
#include "ofp/yaml/ybytelist.h"
#if HAVE_LIBPCAP
#include "ofp/demux/messagesource.h"
#include "ofp/demux/pktsource.h"
#endif

using namespace ofpx;
using ofp::UInt8;
using ExitStatus = Decode::ExitStatus;

static size_t findDiffOffset(const UInt8 *lhs, const UInt8 *rhs, size_t size);

int Decode::run(int argc, const char *const *argv) {
  parseCommandLineOptions(argc, argv,
                          "Translate binary OpenFlow messages in the input "
                          "files to human-readable YAML\n");

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

  // Put opening '[' only if format is json array.
  if (!silent_ && jsonArray_) {
    jsonArrayNeedComma_ = false;
    *output_ << "[\n";
  }

  // If --pcap-device option is specified, decode from packet capture device.
  // Otherwise, decode messages from pcap or binary files.

  ExitStatus result;
  if (!pcapDevice_.empty()) {
    result = decodePcapDevice(pcapDevice_);
  } else if (pcapFormat()) {
    result = decodePcapFiles();
  } else {
    result = decodeFiles();
  }

  // Put closing ']' only if format is json array.
  if (!silent_ && jsonArray_) {
    *output_ << "]\n";
  }

  return static_cast<int>(result);
}

bool Decode::validateCommandLineArguments() {
  // It's an error to specify any input files in combination with live packet
  // capture.
  if (!pcapDevice_.empty() && !inputFiles_.empty()) {
    llvm::errs() << "Error: File list provided with live packet capture\n";
    return false;
  }

  // If `pcapOutputDir_` is specified, it must exist.
  if (!pcapOutputDir_.empty() && !llvm::sys::fs::is_directory(pcapOutputDir_)) {
    llvm::errs() << "Error: Directory " << pcapOutputDir_
                 << " does not exist\n";
    return false;
  }

  // If `pktWriteFile_` is specified, open the `pktSinkFile_`.
  if (!pktWriteFile_.empty()) {
#if HAVE_LIBPCAP
    pktSinkFile_ = ofp::MakeUniquePtr<ofp::demux::PktSink>();
    if (!pktSinkFile_->openFile(pktWriteFile_)) {
      llvm::errs() << "Error: " << pktSinkFile_->error() << '\n';
      return false;
    }
#else
    llvm::errs() << "Error: libpcap not supported\n";
    return false;
#endif
  }

  // If there are no input files, add "-" to indicate stdin.
  if (inputFiles_.empty()) {
    inputFiles_.push_back("-");
  }

  // If output file is empty, add "-" to indicate stdout.
  if (outputFile_.empty()) {
    outputFile_ = "-";
  }

  // --json-array implies --json.
  if (!silent_ && jsonArray_) {
    json_ = true;
  }

  // Parse --msg-exclude and --msg-include filters.
  parseMsgFilter(msgExclude_, &msgExcludeFilter_);
  parseMsgFilter(msgInclude_, &msgIncludeFilter_);

  // Parse --pkt-filter filter for PacketIn's and PacketOut's.
  if (!pktFilter_.empty() && !pktIncludeFilter_.setFilter(pktFilter_)) {
    llvm::errs() << "Error: invalid pcap filter: " << pktFilter_ << '\n';
    return false;
  }

  return true;
}

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

ExitStatus Decode::decodeFile(const std::string &filename) {
  log_debug("decodeFile:", filename);

  std::istream *input = nullptr;
  std::ifstream file;
  if (filename != "-") {
    // Check if filename is a directory.
    if (llvm::sys::fs::is_directory(filename)) {
      llvm::errs() << "Error: can't open directory: " << filename << '\n';
      return ExitStatus::FileOpenFailed;
    }
    file.open(filename, std::ifstream::binary);
    input = &file;
  } else {
    input = &std::cin;
  }

  if (!*input) {
    llvm::errs() << "Error: opening file " << filename << '\n';
    return ExitStatus::FileOpenFailed;
  }

  setCurrentFilename(filename);
  ExitStatus result = decodeMessages(*input);

  setCurrentFilename("");

  return result;
}

ExitStatus Decode::decodeMessages(std::istream &input) {
  // Create message buffers.
  ofp::Message message{nullptr};
  ofp::Message originalMessage{nullptr};
  ofp::Timestamp timestamp;

  message.setInfo(&sessionInfo_);

  while (input) {
    // Read the message header.
    char *msg = reinterpret_cast<char *>(
        message.mutableDataResized(sizeof(ofp::Header)));

    input.read(msg, sizeof(ofp::Header));
    if (!input) {
      return checkError(input, sizeof(ofp::Header), true);
    }

    if (timestampFormat_ > kTimestampNone) {
      timestamp = ofp::Timestamp::now();
    }

    // Check header length in message. If the header says the length is less
    // than 8 bytes, the entire message *must* still be 8 bytes in length.
    size_t msgLen = message.header()->length();
    if (msgLen < sizeof(ofp::Header)) {
      msgLen = sizeof(ofp::Header);
    }

    // Read the message body.
    msg = reinterpret_cast<char *>(message.mutableDataResized(msgLen));
    std::streamsize bodyLen = ofp::Signed_cast(msgLen - sizeof(ofp::Header));

    input.read(msg + sizeof(ofp::Header), bodyLen);
    if (!input) {
      return checkError(input, bodyLen, false);
    }

    // Save a copy of the original message binary before we normalize it
    // for parsing. After we decode the message, we'll re-encode it and
    // compare it to this original.

    originalMessage.assign(message);
    message.normalize();
    message.setTime(timestamp);

    ExitStatus result = decodeOneMessage(&message, &originalMessage);
    if (result != ExitStatus::Success && !keepGoing_) {
      return result;
    }
  }

  return ExitStatus::Success;
}

ExitStatus Decode::decodePcapDevice(const std::string &device) {
#if HAVE_LIBPCAP
  ofp::demux::PktSource pcap;
  ofp::demux::MessageSource msg{pcapMessageCallback,  this,
                                pcapOutputDir_,       pcapSkipPayload_,
                                pcapMaxMissingBytes_, pcapConvertPacketIn_};

  if (!pcap.openDevice(device.c_str(), pcapFilter_)) {
    llvm::errs() << "Error: " << pcap.error() << '\n';
    // If device is not found, list all available devices.
    if (llvm::StringRef{pcap.error()}.endswith("No such device")) {
      std::string devices;
      if (pcap.getDeviceList(&devices)) {
        llvm::errs() << "Available devices:\n" << devices << '\n';
      }
    }

    return ExitStatus::FileOpenFailed;
  }

  llvm::errs() << "Listening on " << device << ", link-type " << pcap.datalink()
               << ", filter \"" << pcapFilter_ << "\"\n";

  setCurrentFilename(std::string("pcap:") + device);
  msg.runLoop(&pcap);
  pcap.close();
  setCurrentFilename("");

  return ExitStatus::Success;
#else
  llvm::errs() << "Error: libpcap not supported\n";
  return ExitStatus::UnsupportedFeature;
#endif
}

ExitStatus Decode::decodePcapFiles() {
#if HAVE_LIBPCAP
  const std::vector<std::string> &files = inputFiles_;

  // Use the same MessageSource object for all files, so we can stitch
  // together TCP streams that may cross over files.

  ofp::demux::PktSource pcap;
  ofp::demux::MessageSource msg{pcapMessageCallback,  this,
                                pcapOutputDir_,       pcapSkipPayload_,
                                pcapMaxMissingBytes_, pcapConvertPacketIn_};

  for (auto &filename : files) {
    // Try to read the file as a .pcap file.
    if (!pcap.openFile(filename, pcapFilter_)) {
      llvm::errs() << "Error: " << filename << ": " << pcap.error() << '\n';
      return ExitStatus::FileOpenFailed;
    }

    llvm::errs() << "Reading from \"" << filename << "\", link-type "
                 << pcap.datalink() << ", filter \"" << pcapFilter_ << "\"\n";

    setCurrentFilename(filename);
    msg.runLoop(&pcap);
    pcap.close();
    setCurrentFilename("");
  }

  llvm::errs() << pcap.packetCount() << " packets processed from "
               << files.size() << " file(s).";
  if (pcap.packetCount() == 0 && !pcapFilter_.empty()) {
    llvm::errs() << " Use --pcap-filter='' to process all packets.\n";
  } else {
    llvm::errs() << '\n';
  }

  return ExitStatus::Success;
#else
  llvm::errs() << "Error: libpcap not supported\n";
  return ExitStatus::UnsupportedFeature;
#endif
}

ExitStatus Decode::checkError(std::istream &input, std::streamsize readLen,
                              bool header) {
  assert(!input);

  if (!input.eof()) {
    // Premature I/O error; we're not at EOF.
    // FIXME: print out the error
    llvm::errs() << "Filename: " << currentFilename_ << ":\n";
    llvm::errs() << "Error: I/O error reading from file\n";
    return ExitStatus::MessageReadFailed;
  } else if (input.gcount() != readLen && !(header && input.gcount() == 0)) {
    // EOF and insufficient input remaining. N.B. Zero bytes of header read at
    // EOF is a normal exit condition.
    llvm::errs() << "Filename: " << currentFilename_ << ":\n";
    const char *what = header ? "header" : "body";
    llvm::errs() << "Error: Only " << input.gcount()
                 << " bytes read of message " << what << ". Expected to read "
                 << readLen << " bytes.\n";
    return ExitStatus::MessageReadFailed;
  } else {
    // EOF and everything is good.
    return ExitStatus::Success;
  }
}

ExitStatus Decode::decodeOneMessage(const ofp::Message *message,
                                    const ofp::Message *originalMessage) {
  if (!isMsgTypeAllowed(message)) {
    // Ignore message based on type.
    log_debug("decodeOneMessage (message ignored)", message->type());
    return ExitStatus::Success;
  }

  const bool hasPkt = (message->type() == ofp::OFPT_PACKET_IN ||
                       message->type() == ofp::OFPT_PACKET_OUT);
  if (hasPkt && !isPktDataAllowed(message)) {
    // Ignore message based on packet_in/packet_out contents.
    log_debug("decodeOneMessage (packet message ignored)");
    return ExitStatus::Success;
  }

  log_debug("decodeOneMessage (normalized):", *message);

  ofp::yaml::Decoder decoder{message, json_, pktDecode_};

  if (!decoder.error().empty()) {
    // An error occurred in decoding the message.

    if (invertCheck_) {
      // We're expecting invalid data -- report no error and continue.
      return ExitStatus::Success;
    }

    if (!silentError_) {
      llvm::errs() << "Filename: " << currentFilename_ << '\n';
      llvm::errs() << "Error: Decode failed: " << decoder.error() << '\n';
      llvm::errs() << *originalMessage << '\n';
    }

    return ExitStatus::DecodeFailed;
  }

  if (invertCheck_) {
    // There was no problem decoding the message, but we are expecting the data
    // to be invalid (because we are fuzz testing). Report this as an error.
    if (!silentError_) {
      llvm::errs() << "Filename: " << currentFilename_ << '\n';
      llvm::errs()
          << "Error: Decode succeeded when --invert-check flag is specified.\n";
      llvm::errs() << *originalMessage << '\n';
    }
    return ExitStatus::DecodeSucceeded;
  }

  if (!silent_) {
    if (jsonArray_ && jsonArrayNeedComma_) {
      *output_ << ',';
    }
    *output_ << decoder.result();
    if (json_) {
      *output_ << '\n';
      jsonArrayNeedComma_ = true;
    }
    output_->flush();
  }

  // Double-check the result by re-encoding the YAML message.
  if (verifyOutput_ && !verifyOutput(decoder.result(), originalMessage)) {
    return ExitStatus::VerifyOutputFailed;
  }

  // Optionally, write data from PacketIn or PacketOut messages.
  if (pktSinkFile_ && hasPkt) {
    extractPacketDataToFile(message);
  }

  // Optionally run the original message through a basic fuzz test to stress
  // test the decoder.
  if (fuzzStressTest_) {
    fuzzStressTest(originalMessage);
  }

  return ExitStatus::Success;
}

/// Parse msgType filter -- separate on commas.
void Decode::parseMsgFilter(const std::string &input,
                            std::vector<std::string> *filter) {
  llvm::SmallVector<llvm::StringRef, 5> vals;
  llvm::StringRef{input}.split(vals, ',', -1, false);

  for (const auto &s : vals) {
    filter->push_back(s.str());
  }
}

/// Helper function to compare string pattern to endpoint. Currently, we only
/// compare the port.
static bool matchEndpoint(llvm::StringRef pattern,
                          const ofp::IPv6Endpoint &endpt) {
  ofp::UInt16 port = 0;
  if (pattern.getAsInteger(0, port))
    return false;
  return port == endpt.port();
}

/// Helper function to compare string pattern to conn_id.
static bool matchConnId(llvm::StringRef pattern, ofp::UInt64 connId) {
  ofp::UInt64 conn = 0;
  if (pattern.getAsInteger(0, conn))
    return false;
  return conn == connId;
}

/// Return true if pattern matches this message.
///
/// `msgType` is passed in pre-computed. If pattern begins with '!', negate
/// the result.
static bool matchMessage(const char *pattern, const ofp::Message *message,
                         const char *msgType) {
  assert(pattern);
  assert(msgType);

  bool negate = false;
  if (*pattern == '!') {
    negate = true;
    ++pattern;
  }

  llvm::StringRef pat{pattern};
  bool result = false;

  if (pat.startswith("src:")) {
    // "src:<port>" matches messages from <port>
    ofp::MessageInfo *info = message->info();
    if (!info)
      return false;
    result = matchEndpoint(pat.substr(4), info->source());
  } else if (pat.startswith("dst:")) {
    // "dst:<port>" matches messages to <port>
    ofp::MessageInfo *info = message->info();
    if (!info)
      return false;
    result = matchEndpoint(pat.substr(4), info->dest());
  } else if (pat.startswith("conn_id:")) {
    // "conn_id:<id>" matches message for conn_id <id>
    ofp::MessageInfo *info = message->info();
    if (!info)
      return false;
    result = matchConnId(pat.substr(8), info->sessionId());
  } else {
    // Test pattern as glob against message type.
    result = (fnmatch(pattern, msgType, FNM_CASEFOLD) == 0);
  }

  return negate ? !result : result;
}

/// Return true if we're allowed to output this message type.
bool Decode::isMsgTypeAllowed(const ofp::Message *message) const {
  // No filters?  Allow everything.
  if (msgExcludeFilter_.empty() && msgIncludeFilter_.empty())
    return true;

  // Get message type as a string, exactly as we would output it.
  std::string buf;
  llvm::raw_string_ostream os{buf};
  llvm::yaml::ScalarTraits<ofp::MessageType>::output(message->msgType(),
                                                     nullptr, os);
  auto msgType = os.str();

  // Check msgType against the exclude filter.
  for (const auto &pattern : msgExcludeFilter_) {
    if (matchMessage(pattern.c_str(), message, msgType.c_str()))
      return false;
  }

  // Empty include filter means allow everything that's not excluded.
  if (msgIncludeFilter_.empty())
    return true;

  // Check msgType against the include filter.
  for (const auto &pattern : msgIncludeFilter_) {
    if (matchMessage(pattern.c_str(), message, msgType.c_str()))
      return true;
  }

  return false;
}

/// Return true if we're allowed to output this packet.
bool Decode::isPktDataAllowed(const ofp::Message *message) const {
  using namespace ofp;

  if (pktIncludeFilter_.empty())
    return true;

  if (message->type() == OFPT_PACKET_IN) {
    const PacketIn *packetIn = PacketIn::cast(message);
    if (packetIn) {
      return pktIncludeFilter_.match(packetIn->enetFrame(),
                                     packetIn->totalLen());
    }

  } else if (message->type() == OFPT_PACKET_OUT) {
    const PacketOut *packetOut = PacketOut::cast(message);
    if (packetOut) {
      return pktIncludeFilter_.match(packetOut->enetFrame());
    }
  }

  return true;
}

/// Return true if the two messages are equal.
bool Decode::equalMessages(ofp::ByteRange origData,
                           ofp::ByteRange newData) const {
  ofp::ByteList buf;
  if (normalizeTableFeaturesMessage(origData, buf)) {
    log_debug("equalMessage: normalized TableFeatures message");
    origData = buf.toRange();
  }

  // First compare the size of the messages.
  if (origData.size() != newData.size()) {
    llvm::errs() << "Filename: " << currentFilename_ << '\n';
    llvm::errs() << "Error: Encode yielded different size data: "
                 << newData.size() << " vs. " << origData.size() << '\n'
                 << newData << '\n'
                 << origData << '\n';
    return false;
  }

  assert(origData.size() == newData.size());

  // Next, compare the actual contents of the messages.
  if (std::memcmp(origData.data(), newData.data(), newData.size()) != 0) {
    size_t diffOffset =
        findDiffOffset(origData.data(), newData.data(), newData.size());
    llvm::errs() << "Filename: " << currentFilename_ << '\n';
    llvm::errs() << "Error: Encode yielded different data at byte offset "
                 << diffOffset << ":\n"
                 << newData << '\n'
                 << origData << '\n';
    return false;
  }

  return true;
}

/// Update the current file. To clear the current file, pass "".
void Decode::setCurrentFilename(const std::string &filename) {
  currentFilename_ = filename;

  if (filename.empty()) {
    sessionInfo_ = ofp::MessageInfo{};
    return;
  }

  if (showFilename_) {
    sessionInfo_ = ofp::MessageInfo{filename};
  } else {
    sessionInfo_ = ofp::MessageInfo{};
  }
}

void Decode::pcapMessageCallback(ofp::Message *message, void *context) {
  Decode *decode = reinterpret_cast<Decode *>(context);

  // Save a copy of the original message binary before we normalize it
  // for parsing.
  ofp::Message originalMessage{nullptr};
  originalMessage.assign(*message);
  message->normalize();

  ExitStatus result = decode->decodeOneMessage(message, &originalMessage);
  if (result != ExitStatus::Success) {
    log_debug("pcapMessageCallback: Failed to decode message");
  }
}

/// Return true if file has pcap magic header.
static bool hasPcapMagicHeader(const std::string &fname) {
  const ofp::UInt32 PCAP_MAGIC = 0xa1b2c3d4;
  bool result = false;
  FILE *file = std::fopen(fname.c_str(), "rb");
  if (file) {
    ofp::UInt32 magic = 0;
    if (std::fread(&magic, 1, sizeof(magic), file) == sizeof(magic)) {
      log_debug("hasPcapMagicHeader: magic", magic);
      result = (magic == PCAP_MAGIC) ||
               (ofp::detail::SwapByteOrder(magic) == PCAP_MAGIC);
    }
    std::fclose(file);
  }
  return result;
}

bool Decode::pcapFormat() const {
  if (pcapFormat_ == kPcapFormatNo)
    return false;

  if (pcapFormat_ == kPcapFormatYes)
    return true;

  assert(pcapFormat_ == kPcapFormatAuto);

  // Check if any of the input files have the .pcap file extension.
  for (const auto &filename : inputFiles_) {
    llvm::StringRef fname{filename};
    if (fname.endswith(".pcap") || fname.endswith(".pcapng") ||
        fname.endswith(".cap")) {
      return true;
    }
  }

  // Check for PCAP file signature (first 4 bytes) in the first file.
  if (!inputFiles_.empty()) {
    if (hasPcapMagicHeader(inputFiles_[0])) {
      return true;
    }
  }

  return false;
}

// Double-check the result by re-encoding the YAML message. We should obtain
// the original message contents. If there is a difference, report the
// error.
bool Decode::verifyOutput(const std::string &input,
                          const ofp::Message *originalMessage) {
  ofp::yaml::Encoder encoder{input, false};

  if (!encoder.error().empty()) {
    llvm::errs() << "Filename: " << currentFilename_ << '\n';
    llvm::errs() << "Error: Decode succeeded but encode failed: "
                 << encoder.error() << '\n';
    return false;
  }

  if (!equalMessages({originalMessage->data(), originalMessage->size()},
                     {encoder.data(), encoder.size()})) {
    return false;
  }

  return true;
}

// If message is a PacketIn or PacketOut, write it's data payload to a .pcap
// file `pktSinkFile_`. Don't write the data from a PacketOut message if the
// data is empty.
void Decode::extractPacketDataToFile(const ofp::Message *message) {
#if HAVE_LIBPCAP
  using namespace ofp;

  if (message->type() == OFPT_PACKET_IN) {
    const PacketIn *packetIn = PacketIn::cast(message);
    if (packetIn) {
      pktSinkFile_->write(message->time(), packetIn->enetFrame(),
                          packetIn->totalLen());
    }

  } else if (message->type() == OFPT_PACKET_OUT) {
    const PacketOut *packetOut = PacketOut::cast(message);
    if (packetOut) {
      ByteRange enetFrame = packetOut->enetFrame();
      UInt32 totalLen = UInt32_narrow_cast(enetFrame.size());
      if (totalLen > 0) {
        pktSinkFile_->write(message->time(), enetFrame, totalLen);
      }
    }
  }
#endif
}

// Run a simple fuzz test on the original message.
//
// 1. Treat the message as a different type (2nd byte, OFPT_MAX_ALLOWED).
// 2. Change one post-header byte at a time:
//     a. Set byte to 0x00
//     b. Set byte to 0xFF
//
void Decode::fuzzStressTest(const ofp::Message *originalMessage) {
  using namespace ofp;
  Message message{nullptr};
  UInt64 count = 0;

  for (UInt8 newType = 0; newType <= OFPT_MAX_ALLOWED; ++newType) {
    if (newType != originalMessage->type()) {
      message.assign(*originalMessage);
      message.mutableHeader()->setType(static_cast<OFPType>(newType));
      SetWatchdogTimer(3);
      message.normalize();
      yaml::Decoder decoder{&message, json_, pktDecode_};
      if ((++count % 100) == 0) {
        log_info("fuzz-stress-test:", count);
      }
    }
  }

  const UInt8 values[] = {0x00, 0xFF};

  // Only fuzz the first 128 bytes.
  const size_t kFuzzPrefix = 128;
  const size_t kMaxSize =
      std::min(originalMessage->size(), kFuzzPrefix + sizeof(Header));

  for (size_t i = sizeof(Header); i < kMaxSize; ++i) {
    for (UInt8 val : values) {
      if (originalMessage->getByteAtIndex(i) != val) {
        message.assign(*originalMessage);
        message.setByteAtIndex(val, i);
        SetWatchdogTimer(3);
        message.normalize();
        yaml::Decoder decoder{&message, json_, pktDecode_};
        if ((++count % 100) == 0) {
          log_info("fuzz-stress-test:", count);
        }
      }
    }
  }

  SetWatchdogTimer(0);
}

// Compare two buffers and return offset of the byte that differs. If buffers
// are identical, return `size`.
static size_t findDiffOffset(const UInt8 *lhs, const UInt8 *rhs, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    if (lhs[i] != rhs[i])
      return i;
  }
  return size;
}
