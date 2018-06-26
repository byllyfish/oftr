// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef TOOLS_OFTR_OFTR_H_
#define TOOLS_OFTR_OFTR_H_

#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "ofp/ofp.h"

namespace ofpx {

namespace cl = llvm::cl;

class Subprogram {
 public:
  virtual ~Subprogram() {}
  virtual int run(int argc, const char *const *argv) = 0;

 protected:
  static const int UnsupportedFeatureExitStatus = 7;
  static const int InvalidArgumentsExitStatus = 8;
  static const int FileOpenFailedExitStatus = 9;
  static const int MinExitStatus = 10;

  void parseCommandLineOptions(int argc, const char *const *argv,
                               const char *overview) {
    cl::ParseCommandLineOptions(argc, argv, overview);

    if (helpAlias_) {
      cl::PrintHelpMessage(false, true);
    }

    setupLogging();

    if (initialSleep_ > 0) {
      // Sleep immediately to allow a debugger or performance tool to attach.
      log_info("Initial sleep for", initialSleep_, "seconds");
      ::sleep(initialSleep_);
    }
  }

  void setupLogging() {
    using namespace llvm::sys;
    std::unique_ptr<llvm::raw_ostream> logStream;

    if (!logfile_.empty()) {
      std::error_code err;
      logStream.reset(new llvm::raw_fd_ostream{
          logfile_, err, fs::F_Append | fs::F_Text});
      if (err) {
        llvm::errs() << "oftr: Failed to open log file '" << logfile_ << "'\n";
        std::exit(1);
      }

    } else {
      logStream.reset(new llvm::raw_fd_ostream{2, true});
    }

    ofp::log::configure(loglevel_,
                        static_cast<ofp::log::Trace>(logtrace_.getBits()),
                        std::move(logStream));
  }

  // --- Command-line Arguments ---
  cl::opt<bool> helpAlias_{"h", cl::desc("Alias for -help"), cl::Grouping,
                           cl::Hidden};
  cl::OptionCategory logCategory_{"Logging Options"};
  cl::opt<ofp::log::Level> loglevel_{
      "loglevel",
      cl::desc("Log level (exactly one)"),
      cl::ValueRequired,
      cl::Hidden,
      cl::cat(logCategory_),
      cl::init(ofp::log::Level::Fatal),
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
                            "Log fatal messages only - the default"))};
  cl::opt<std::string> logfile_{
      "logfile", cl::desc("Log messages to this file"), cl::ValueRequired,
      cl::Hidden, cl::cat(logCategory_)};
  cl::bits<ofp::log::Trace> logtrace_{
      "trace",
      cl::desc("Trace flags (one or more separated by commas)"),
      cl::CommaSeparated,
      cl::Hidden,
      cl::cat(logCategory_),
      cl::values(clEnumValN(ofp::log::kTraceMsg, "msg",
                            "Log all OpenFlow messages sent and received"),
                 clEnumValN(ofp::log::kTraceRpc, "rpc",
                            "Log all JSON-RPC events sent and received"))};
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

#endif  // TOOLS_OFTR_OFTR_H_
