// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include <iostream>
#include "./testagent.h"

using namespace testagent;

static UInt64 addEnvIdentity(Driver &driver, const std::string &envPrefix) {
  std::string envCert = envPrefix + "_CERT";
  std::string envPrivKey = envPrefix + "_PRIVKEY";
  std::string envCACert = envPrefix + "_CACERT";

  const char *cert = getenv(envCert.c_str());
  const char *privKey = getenv(envPrivKey.c_str());
  const char *caCert = getenv(envCACert.c_str());

  if (!cert) {
    return 0;
  }

  if (!privKey) {
    privKey = cert;
  }

  if (!caCert) {
    caCert = "";
  }

  std::error_code err;
  UInt64 result = driver.addIdentity(cert, privKey, caCert, "", "", "", err);

  if (err) {
    log_error("Unable to add TLS identity:", err);
    ::exit(1);
  }

  return result;
}

int main(int argc, char **argv) {
  std::vector<std::string> args{argv + 1, argv + argc};

  IPv6Endpoint remoteEndpoint;
  if (!args.empty()) {
    if (!remoteEndpoint.parse(args[0])) {
      llvm::errs() << "testagent: Argument 1 is not an endpoint: `" << args[0]
                   << "`\n";
      return 1;
    }
  }

  std::error_code error;
  Driver driver;
  driver.installSignalHandlers();

  UInt64 securityId = addEnvIdentity(driver, "AGENT");

  if (remoteEndpoint.valid()) {
    (void)driver.connect(
        ChannelOptions::NONE, securityId, remoteEndpoint, {OFP_VERSION_1},
        TestAgent::Factory,
        [&error, &remoteEndpoint](Channel *channel, std::error_code err) {
          if (err) {
            llvm::errs() << "testagent: Error connecting to `" << remoteEndpoint
                         << "`: connId=" << channel->connectionId()
                         << " err=" << err.message() << '\n';
          }
          error = err;
        });

  } else {
    (void)driver.listen(ChannelOptions::NONE, securityId,
                        IPv6Endpoint{OFPGetDefaultPort()}, {OFP_VERSION_1},
                        TestAgent::Factory, error);
  }

  driver.run();

  if (error) {
    llvm::errs() << "testagent: Error starting agent: " << error.message()
                 << '\n';
    return 1;
  }

  return 0;
}
