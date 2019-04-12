// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "./simplechannellistener.h"
#include "ofp/ofp.h"

using namespace ofp;


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
  log::configure(log::Level::Debug);

  Driver driver;
  driver.installSignalHandlers();

  UInt64 securityId = addEnvIdentity(driver, "CONTROLLER");

  std::error_code err;
  (void)driver.listen(
      ChannelOptions::FEATURES_REQ, securityId, IPv6Endpoint{OFPGetDefaultPort()},
      ProtocolVersions::All,
      []() { return new controller::SimpleChannelListener; }, err);

  driver.run();

  if (err) {
    log_error("Error running controller:", err);
    return 1;
  }

  return 0;
}
