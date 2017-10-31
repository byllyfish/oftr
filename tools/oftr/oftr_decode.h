// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef TOOLS_OFTR_OFTR_DECODE_H_
#define TOOLS_OFTR_OFTR_DECODE_H_

#include <map>
#include "./oftr.h"
#include "ofp/messageinfo.h"
#include "ofp/timestamp.h"

#if HAVE_LIBPCAP
#include "ofp/demux/pktsink.h"
#include "ofp/demux/pktfilter.h"
#else
namespace ofp {
namespace demux {
class PktSink {};
}  // namespace demux
}  // namespace ofp
#endif

namespace ofpx {

// oftr decode [<options>] [<Input files>]
//
// Decode binary OpenFlow messages in the input files and translate each
// message to human-readable YAML output. If there is an invalid message,
// stop and report an error.
//
// If no input files are specified, use standard input (stdin). A single hyphen
// also represents stdin.
//
//   --json (-j)           Write compact JSON output instead of YAML.
//   --json-array          Write output as a valid JSON array.
//   --json-flavor=default|mongodb Flavor of JSON output
//   --silent (-s)         Quiet mode; suppress normal output.
//   --silent-error        Suppress error output for invalid messages.
//   --invert-check (-v)   Expect invalid messages only.
//   --keep-going (-k)     Continue processing messages after errors.
//   --verify-output (-V)  Verify output by translating it back to binary.
//   --use-findx           Use metadata from tcpflow '.findx' files.
//   --pkt-decode          Include _pkt in PacketIn/PacketOut decodes.
//   --pkt-write-file=<file> Write data from PacketIn/PacketOut messages to
//   .pcap file.
//   --show-filename       Show the file name in all decodes.
//   --output=<file> (-o)  Write output to specified file instead of stdout.
//   --pcap-device=<device> Reassemble OpenFlow messages from specified device.
//   --pcap-filter=<filter>  Filter for packet capture.
//   --pcap-format=auto|yes|no Treat input files as .pcap format.
//   --pcap-output-dir=<dir> Write reassembled TCP streams to <dir> (for
//   debugging).
//   --pcap-skip-payload   Skip payload from TCP streams (for debugging).
//   --pcap-max-missing-bytes=<num>  Add missing zero bytes to partial streams
//   (for debugging).
//   --pcap-convert-packetin  Convert captured packets to PacketIn messages.
//   --msg-include=<types> Output these OpenFlow message types (glob).
//   --msg-exclude=<types> Don't output these OpenFlow message types (glob).
//   --pkt-filter=<filter> Filter packets inside PacketIn/PacketOut messages.
//   --timestamp=none|secs Show timestamp in all decodes.
//
// Usage:
//
// To decode a file of binary OpenFlow messages to YAML:
//
//     oftr decode "filename"
//
// To decode a file of binary OpenFlow messages to JSON:
//
//     oftr decode --json "filename"
//
// To process a file of invalid binary OpenFlow messages (e.g. fuzz test). This
// will report an error if any message is structurally valid as an OpenFlow
// message:
//
//     oftr decode --invert-check "filename"
//

OFP_BEGIN_IGNORE_PADDING

class Decode : public Subprogram {
 public:
  enum class ExitStatus {
    Success = 0,
    UnsupportedFeature = UnsupportedFeatureExitStatus,
    InvalidArguments = InvalidArgumentsExitStatus,
    FileOpenFailed = FileOpenFailedExitStatus,
    DecodeFailed = MinExitStatus,
    DecodeSucceeded,
    VerifyOutputFailed,
    MessageReadFailed,
    IndexReadFailed,
  };

  int run(int argc, const char *const *argv) override;

 private:
  std::string currentFilename_;
  std::unique_ptr<llvm::raw_ostream> output_;
  ofp::MessageInfo sessionInfo_;
  bool jsonArrayNeedComma_ = false;

  using EndpointPair = std::pair<ofp::IPv6Endpoint, ofp::IPv6Endpoint>;
  std::map<EndpointPair, ofp::UInt64> sessionIdMap_;
  ofp::UInt64 nextSessionId_ = 0;

  std::unique_ptr<ofp::demux::PktSink> pktSinkFile_;
  std::vector<std::string> msgIncludeFilter_;
  std::vector<std::string> msgExcludeFilter_;
  ofp::demux::PktFilter pktIncludeFilter_;

  bool validateCommandLineArguments();

  ExitStatus decodeFiles();
  ExitStatus decodeFile(const std::string &filename);
  ExitStatus decodeMessages(std::istream &input);
  ExitStatus decodeMessagesWithIndex(std::istream &input, std::istream &index);
  ExitStatus decodePcapDevice(const std::string &device);
  ExitStatus decodePcapFiles();
  ExitStatus checkError(std::istream &input, std::streamsize readLen,
                        bool header);
  ExitStatus decodeOneMessage(const ofp::Message *message,
                              const ofp::Message *originalMessage);

  static void parseMsgFilter(const std::string &input,
                             std::vector<std::string> *filter);
  bool isMsgTypeAllowed(const ofp::Message *message) const;
  bool isPktDataAllowed(const ofp::Message *message) const;
  bool equalMessages(ofp::ByteRange origData, ofp::ByteRange newData) const;

  static bool parseIndexLine(const llvm::StringRef &line, size_t *pos,
                             ofp::Timestamp *timestamp, size_t *length);

  void setCurrentFilename(const std::string &filename);
  bool parseFilename(const std::string &filename, ofp::MessageInfo *info);
  ofp::UInt64 lookupSessionId(const ofp::IPv6Endpoint &src,
                              const ofp::IPv6Endpoint &dst);

  static void pcapMessageCallback(ofp::Message *message, void *context);
  bool pcapFormat() const;

  bool verifyOutput(const std::string &input,
                    const ofp::Message *originalMessage);
  void extractPacketDataToFile(const ofp::Message *message);
  void fuzzStressTest(const ofp::Message *originalMessage);

  enum PcapFormat { kPcapFormatAuto, kPcapFormatYes, kPcapFormatNo };
  enum JsonFlavor { kJsonFlavorDefault, kJsonFlavorMongoDB };
  enum TimestampFormat { kTimestampUnset, kTimestampNone, kTimestampSecs };

  // --- Command-line Arguments (Order is important here.) ---
  cl::opt<bool> json_{"json",
                      cl::desc("Write compact JSON output instead of YAML")};
  cl::opt<bool> jsonArray_{"json-array",
                           cl::desc("Write output as a valid JSON array")};
  cl::opt<JsonFlavor> jsonFlavor_{
      "json-flavor", cl::desc("Flavor of JSON output"),
      cl::values(clEnumValN(kJsonFlavorDefault, "default", "JSON (default)"),
                 clEnumValN(kJsonFlavorMongoDB, "mongodb", "MongoDB JSON")),
      cl::init(kJsonFlavorDefault)};
  cl::opt<bool> silent_{"silent",
                        cl::desc("Quiet mode; suppress normal output")};
  cl::opt<bool> silentError_{
      "silent-error", cl::desc("Suppress error output for invalid messages")};
  cl::opt<bool> invertCheck_{"invert-check",
                             cl::desc("Expect invalid messages only")};
  cl::opt<bool> keepGoing_{
      "keep-going", cl::desc("Continue processing messages after errors")};
  cl::opt<bool> verifyOutput_{
      "verify-output",
      cl::desc("Verify output by translating it back to binary")};
  cl::opt<bool> fuzzStressTest_{
      "fuzz-stress-test",
      cl::desc("Stress test the decoder by fuzzing the input"), cl::Hidden};
  cl::opt<bool> useFindx_{"use-findx",
                          cl::desc("Use metadata from tcpflow '.findx' files"),
                          cl::Hidden};
  cl::opt<bool> pktDecode_{
      "pkt-decode", cl::desc("Include _pkt in PacketIn/PacketOut decodes")};
  cl::opt<std::string> pktWriteFile_{
      "pkt-write-file",
      cl::desc("Write data from PacketIn/PacketOut messages to .pcap file"),
      cl::ValueRequired};
  cl::opt<bool> showFilename_{"show-filename",
                              cl::desc("Show the file name in all decodes")};
  cl::opt<TimestampFormat> timestampFormat_{
      "timestamp", cl::desc("Show the timestamp in all decodes"),
      cl::values(clEnumValN(kTimestampNone, "none", "None"),
                 clEnumValN(kTimestampSecs, "secs",
                            "Seconds since January 1, 1970 UTC")),
      cl::init(kTimestampUnset)};
  cl::opt<std::string> outputFile_{
      "output", cl::desc("Write output to specified file instead of stdout"),
      cl::ValueRequired};
  cl::opt<std::string> msgInclude_{
      "msg-include", cl::desc("Output these OpenFlow message types (glob)"),
      cl::ValueRequired};
  cl::opt<std::string> msgExclude_{
      "msg-exclude",
      cl::desc("Don't output these OpenFlow message types (glob)"),
      cl::ValueRequired};
  cl::opt<std::string> pktFilter_{
      "pkt-filter", cl::desc("Filter packets inside PacketIn/PacketOut messages (BPF)"),
      cl::ValueRequired};
  cl::OptionCategory pcapCategory_{"Packet Capture Options"};
  cl::opt<std::string> pcapDevice_{
      "pcap-device",
      cl::desc("Reassemble OpenFlow messages from specified device"),
      cl::ValueRequired, cl::cat(pcapCategory_)};
  cl::opt<PcapFormat> pcapFormat_{
      "pcap-format", cl::desc("Treat all input files as .pcap format"),
      cl::values(clEnumValN(kPcapFormatAuto, "auto",
                            "If any file name ends in .pcap (default)"),
                 clEnumValN(kPcapFormatYes, "yes", "Yes"),
                 clEnumValN(kPcapFormatNo, "no", "No")),
      cl::cat(pcapCategory_), cl::init(kPcapFormatAuto)};
  cl::opt<std::string> pcapOutputDir_{
      "pcap-output-dir",
      cl::desc("Write reassembled TCP streams to directory (for debugging)"),
      cl::cat(pcapCategory_), cl::ValueRequired, cl::Hidden};
  cl::opt<std::string> pcapFilter_{
      "pcap-filter", cl::desc("Filter for packet capture (BPF)"),
      cl::cat(pcapCategory_), cl::init("tcp port 6653 or 6633")};
  cl::opt<bool> pcapSkipPayload_{
      "pcap-skip-payload",
      cl::desc("Skip payload from TCP streams (for debugging)"),
      cl::cat(pcapCategory_), cl::Hidden};
  cl::opt<ofp::UInt32> pcapMaxMissingBytes_{
      "pcap-max-missing-bytes",
      cl::desc("Add missing zero bytes to partial streams (for debugging)"),
      cl::cat(pcapCategory_), cl::Hidden};
  cl::opt<bool> pcapConvertPacketIn_{
      "pcap-convert-packetin",
      cl::desc("Convert captured packets to PacketIn messages"),
      cl::cat(pcapCategory_), cl::Hidden};
  cl::list<std::string> inputFiles_{cl::Positional, cl::desc("<Input files>")};

  // --- Argument Aliases (May be grouped into one argument) ---
  cl::alias jAlias_{"j", cl::desc("Alias for -json"), cl::aliasopt(json_),
                    cl::Grouping};
  cl::alias sAlias_{"s", cl::desc("Alias for -silent"), cl::aliasopt(silent_),
                    cl::Grouping};
  cl::alias vAlias_{"v", cl::desc("Alias for -invert-check"),
                    cl::aliasopt(invertCheck_), cl::Grouping};
  cl::alias kAlias_{"k", cl::desc("Alias for -keep-going"),
                    cl::aliasopt(keepGoing_), cl::Grouping};
  cl::alias VAlias_{"V", cl::desc("Alias for -verify-output"),
                    cl::aliasopt(verifyOutput_), cl::Grouping};
  cl::alias oAlias_{"o", cl::desc("Alias for -output"),
                    cl::aliasopt(outputFile_)};
};

OFP_END_IGNORE_PADDING

}  // namespace ofpx

#endif  // TOOLS_OFTR_OFTR_DECODE_H_
