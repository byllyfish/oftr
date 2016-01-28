// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef TOOLS_LIBOFP_OFPX_H_
#define TOOLS_LIBOFP_OFPX_H_

#include <unistd.h>
#include <vector>
#include <string>
#include <iostream>
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "ofp/ofp.h"

namespace ofpx {

namespace cl = llvm::cl;

class Subprogram {
 public:
  virtual ~Subprogram() {}
  virtual int run(int argc, const char *const *argv) = 0;

 protected:
  static const int FileOpenFailedExitStatus = 9;
  static const int MinExitStatus = 10;

  std::unique_ptr<llvm::raw_ostream> logstream_;

  void parseCommandLineOptions(int argc, const char *const *argv,
                               const char *overview) {
    cl::ParseCommandLineOptions(argc, argv, overview);

    if (helpAlias_) {
      cl::PrintHelpMessage(false, true);
    }

    setupLogging();

    if (initialSleep_ > 0) {
      // Sleep immediately to allow a debugger or performance tool to attach.
      ofp::log::info("Initial sleep for", initialSleep_, "seconds");
      ::sleep(initialSleep_);
    }
  }

  void setupLogging() {
    using namespace llvm::sys;

    if (!logfile_.empty()) {
      std::error_code err;
      logstream_.reset(new llvm::raw_fd_ostream{
          logfile_, err, fs::F_Append | fs::F_RW | fs::F_Text});
      if (err) {
        std::cerr << "libofp: Failed to open log file '" << logfile_ << "'\n";
        std::exit(1);
      }

    } else {
      logstream_.reset(new llvm::raw_fd_ostream{2, true});
    }

    logstream_->SetBufferSize(4096);
    ofp::log::setOutputStream(logstream_.get());

    ofp::log::setOutputLevelFilter(loglevel_);
    ofp::log::setOutputTraceFilter(logtrace_.getBits());
  }

  // --- Command-line Arguments ---
  cl::opt<bool> helpAlias_{"h", cl::desc("Alias for -help"), cl::Grouping,
                           cl::Hidden};
  cl::OptionCategory logCategory_{"Logging Options"};
  cl::opt<ofp::log::Level> loglevel_{
      "loglevel", cl::desc("Log level (exactly one)"), cl::ValueRequired,
      cl::Hidden, cl::cat(logCategory_), cl::init(ofp::log::Level::Fatal),
      cl::values(clEnumValN(ofp::log::Level::Silent, "none",
                            "No log messages emitted"),
                 clEnumValN(ofp::log::Level::Debug, "debug",
                            "Log debug messages and above"),
                 clEnumValN(ofp::log::Level::Info, "info",
                            "Log info messages and above"),
                 clEnumValN(ofp::log::Level::Warning, "warning",
                            "Log warning messages and above"),
                 clEnumValN(ofp::log::Level::Error, "error",
                            "Log error messages and above"),
                 clEnumValN(ofp::log::Level::Fatal, "fatal",
                            "Log fatal messages only - the default"),
                 clEnumValEnd)};
  cl::opt<std::string> logfile_{
      "logfile", cl::desc("Log messages to this file"), cl::ValueRequired,
      cl::Hidden, cl::cat(logCategory_)};
  cl::bits<ofp::log::Trace> logtrace_{
      "trace", cl::desc("Trace flags (one or more separated by commas)"),
      cl::CommaSeparated, cl::Hidden, cl::cat(logCategory_),
      cl::values(clEnumValN(ofp::log::Trace::Msg, "msg",
                            "Log all OpenFlow messages sent and received"),
                 clEnumValN(ofp::log::Trace::Rpc, "rpc",
                            "Log all JSON-RPC events sent and received"),
                 clEnumValEnd)};
  cl::opt<unsigned> initialSleep_{"initial-sleep",
                                  cl::desc("Sleep immediately at startup"),
                                  cl::Hidden, cl::init(0)};
};

using RunSubprogram = int (*)(int argc, const char *const *argv);

template <class Type>
inline int Run(int argc, const char *const *argv) {
  Type t;
  return t.run(argc, argv);
}

struct IPv6EndpointParser : public cl::parser<ofp::IPv6Endpoint> {
 public:
  // parse - Return true on error.
  bool parse(cl::Option &O, llvm::StringRef ArgName, llvm::StringRef ArgValue,
             ofp::IPv6Endpoint &Val) {
    if (Val.parse(ArgValue))
      return false;
    return O.error("Unexpected endpoint format '" + ArgValue + "'!");
  }
};

}  // namespace ofpx

#endif  // TOOLS_LIBOFP_OFPX_H_
