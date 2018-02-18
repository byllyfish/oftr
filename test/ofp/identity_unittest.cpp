// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/sys/identity.h"
#include "ofp/unittest.h"

using namespace ofp;

static const char *const kGarbageCertificate = R"""(
-----BEGIN CERTIFICATE-----
MIIDhzCCAm+gAwIBAgIEB1vNFTANBgkqhkiG9w0BAQsFADBZMR8wHQYDVQQDDBZz
c2wtc2VydmVyLmV4YW1wbGUuY29tMQswCQYDVQQGEwJVUzEpMCcGCSqGSIb3DQEJ
ARYad2lsbGlhbS53LmZpc2hlckBnbWFpbC5jb20wHhcNMTYwMzEwMjIzODExWhcN
MzUwNTEwMjIzODExWjBZMR8wHQYDVQQDDBZzc2wtc2VydmVyLmV4YW1wbGUuY29t
MQswCQYDVQQGEwJVUzEpMCcGCSqGSIb3DQEJARYad2lsbGlhbS53LmZpc2hlckBn
bWFpbC5jb20wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCvR1RyKn29
bEZHYZgaU3C0XR/tjn9FyOrclmYVt8aKGUisJ1JqD9e+mcrZ+4/vzMjuaPDISShf
gGMIdCHvyao2l/a6Yqn/wCCV1eIIm9JKiv0feRpc6ODUeStGux3uTvaZYLJf7Pb5
DVSSZg2U6yJDdO12kpf6QUkGxFwJx5ph7ndAdMVijowJDbbNnSTuIAtEwMWhih4P
8gMyyW9BcF/TykTwpNUjQTdBvEzq0MGjI6Do3MHBNZv3W5lKHkgvquSLsV/w3FEa
P7tGpmGYd/eN5o6w3nd68NpcOSz23PZ7Ab5egPVk0vqOE5M89inu3wLBs50+k3V0
CEcNxuh208ABAgMBAAGjVzBVMA4GA1UdDwEB/wQEAwIHgDAWBgNVHSUBAf8EDDAK
BggrBgEFBQcDATArBgNVHREEJDAighpzc2wtc2VydmVyLWRucy5leGFtcGxlLmNv
bYcEwKg4ATANBgkqhkiG9w0BAQsFAAOCAQEAOZ4rqcIzhEYbZbvSmKtRweB5upeG
qQ8gMtcTUaQESGjHWjD3gtuZS0qH88U7np6rfkrhHppMVByUlwHiT8HD1/j4qt+o
Il48dk6YeJGAtG3QK3x9dRmwoNNx6CZvkRafjJIFA/3hTh5Q/SBJfJt1EZGD5a6+
SF9f6LUbmYfUZgJja3gW7GXSC4y2XqeKdEovkHKOdMNuQ7+JL6pJw6WLohd9EeAP
d0oF/cFsRd1wDJTkbcBrEFafQk9wJG6sEysyT/Vu5vEX4T/QUvdnthKKXuor8PvR
m4cYRyzZQrX//CcqZMyA7I6KovA02cdp1iLMWl5eNPfz5MiVEN2t2rNthw==
-----END CERTIFICATE-----
)""";

static const char *const kGarbagePrivateKey = R"""(
Bag Attributes
    friendlyName: ssl-server.example.com
    localKeyID: 17 06 07 E3 8A D6 EB 11 32 64 1F EA 84 7A 94 27 AB EA CF E9 
Key Attributes: <No Attributes>
-----BEGIN PRIVATE KEY-----
MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCvR1RyKn29bEZH
YZgaU3C0XR/tjn9FyOrclmYVt8aKGUisJ1JqD9e+mcrZ+4/vzMjuaPDISShfgGMI
dCHvyao2l/a6Yqn/wCCV1eIIm9JKiv0feRpc6ODUeStGux3uTvaZYLJf7Pb5DVSS
Zg2U6yJDdO12kpf6QUkGxFwJx5ph7ndAdMVijowJDbbNnSTuIAtEwMWhih4P8gMy
yW9BcF/TykTwpNUjQTdBvEzq0MGjI6Do3MHBNZv3W5lKHkgvquSLsV/w3FEaP7tG
pmGYd/eN5o6w3nd68NpcOSz23PZ7Ab5egPVk0vqOE5M89inu3wLBs50+k3V0CEcN
xuh208ABAgMBAAECggEAKTjYVmVB3yu4t47vS7m19uUz3hW2Pv5KpghFB/SJn+7k
VYCF9GW3cp8OwwdTyaza0J8jiacS9KroH5PYa2F9Q4unvhv4KSrRPwvnNekXoupf
L2i214T5Y17/24mkN2igvF+uc7ryNo55Blkz9oL6Mu0r78f4MCXse8fwcWhdGYys
qiYO98nHo5mEKjLkgWtdF1QZA60NbvN1hm3pY+y0TUIQCKtTMF/JDOeyfV3aB/7W
1OavvojD3gbi9YmseTg0BL57U85A0Ql1DLsCvns/QFrH0+7BtB4aT52jAK/+LpXV
0uGZfgHisMETYuyymLelP7NUY7XSXNbCgmR1/UTl1QKBgQDpYafqidK+22O+XY5E
krBGQPSMASHiW9oY3P7wTtCngtEeehpeHJcfW7kFY2qK3act3qC6J+exnWreO48A
rOyD2ta/z3L8kFYSqnH5NAwPasxlnlZdpPm4UdFvHCQPQ+kPhp55qZXCXF1cAPKk
4Uect2EKl47AizRlz75IvbNTjwKBgQDARBrzO1B2+DIjS2q7JaQdIfDLD9aVfl0r
IUxVj1kkoGSkIkaReR6zWcBBDNmK7v68bKcmOQtwKOWiyJcvaY15zhhC+wA/xa9s
vzdbOr+wVhemPskuswGBtxf05HSXUIqqi9LMQpUfCXigPSALw8JAVNKQJMwqzu5/
tave9Z6rbwKBgCzdqEFPg6NLW+6B39LQ/HBVFm4/3Xq9u7zh52TdUgGSBY+9W4pS
Jr9Ri808YoQVvojz7qQ6/bDImX3lp0Uy+TkT/grb37gsK0/lUonc9qCUIH08pk/U
YmmL2R5qI4iHvqtMbvTmpDK8PPpt+De4U2MOYep+XmQnnsgf2u3zAiGDAoGAX3cx
noZcHMZ+NrIyy7wT8fcxVdKGbPIylsDQ40brL1mIv0rZmU/3o3w0Dtivh+HFkI+u
qzb16/oSk7JgB0W03r8ros54wDGLOkbz2JOiLI+C7KQORubRH7Rm680n6x4tzCLj
R1LkMmYEhCP1roHECSdU6gerbLIVEEqHjYyN/XcCgYEAoyV66CNT/7XzfGZMFcXF
mX2S2+R0JfdlC4p7nl2D6yEHsUY06ZXgtlvCXy/vatuGAhr6mc5ijBGlVmY56Rvp
PbvXb1WZYYvbOYmgM36o2kTqXWXQmdU+celnlGwRyOawmBkJP691qB4PEj4wCQtV
jW5RCAwwl078sGl746tcY30=
-----END PRIVATE KEY-----
)""";

TEST(identity, empty) {
  std::error_code err;
  sys::Identity identity{"", "", "", "", "", "", err};

  log_debug("identity error", err);
  asio::error_code expected{ERR_LIB_PEM, asio::error::get_ssl_category()};
  EXPECT_EQ(expected, err);
}

TEST(identity, invalid_pem) {
  std::error_code err;
  sys::Identity identity{"x", "", "", "", "", "", err};

  log_debug("identity error", err);
  asio::error_code expected{ERR_LIB_PEM, asio::error::get_ssl_category()};
  EXPECT_EQ(expected, err);
}

TEST(identity, cert_only_no_private_key) {
  std::error_code err;
  sys::Identity identity{kGarbageCertificate, "", "", "", "", "", err};

  log_debug("identity error", err);
  asio::error_code expected{ERR_PACK(ERR_LIB_PEM, PEM_R_NO_START_LINE),
                            asio::error::get_ssl_category()};
  EXPECT_EQ(expected, err);
}

TEST(identity, cert_with_private_key_no_ca) {
  std::error_code err;
  sys::Identity identity{
      kGarbageCertificate, kGarbagePrivateKey, "", "", "", "", err};

  // This is an insecure configuration but it's allowed.
  asio::error_code expected;
  EXPECT_EQ(expected, err);
  EXPECT_EQ(SSL_VERIFY_NONE, identity.peerVerifyMode());
}

TEST(identity, cert_with_private_key) {
  std::error_code err;
  sys::Identity identity{kGarbageCertificate,
                         kGarbagePrivateKey,
                         kGarbageCertificate,
                         "",
                         "",
                         "",
                         err};

  asio::error_code expected;
  EXPECT_EQ(expected, err);
  EXPECT_EQ(SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
            identity.peerVerifyMode());

  // EXPECT_EQ(identity.minProtoVersion(), TLS1_2_VERSION);
  // EXPECT_EQ(identity.maxProtoVersion(), TLS1_2_VERSION);
}

TEST(identity, cert_with_invalid_cipher) {
  std::error_code err;
  sys::Identity identity{kGarbageCertificate,
                         kGarbagePrivateKey,
                         kGarbageCertificate,
                         "",
                         "x",
                         "",
                         err};

  log_debug("identity error", err);
  std::error_code expected = std::make_error_code(std::errc::invalid_argument);
  EXPECT_EQ(expected, err);
}

TEST(identity, cert_with_valid_cipher) {
  const char *ciphers = "DEFAULT:+ALL:!SHA1";
  std::error_code err;
  sys::Identity identity{kGarbageCertificate,
                         kGarbagePrivateKey,
                         kGarbageCertificate,
                         "",
                         ciphers,
                         "",
                         err};

  asio::error_code expected;
  EXPECT_EQ(expected, err);
  EXPECT_EQ(SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
            identity.peerVerifyMode());

  // EXPECT_EQ(identity.minProtoVersion(), TLS1_2_VERSION);
  // EXPECT_EQ(identity.maxProtoVersion(), TLS1_2_VERSION);
}

TEST(identity, cert_with_invalid_version) {
  std::error_code err;
  sys::Identity identity{kGarbageCertificate,
                         kGarbagePrivateKey,
                         kGarbageCertificate,
                         "TLX1.9",
                         "",
                         "",
                         err};

  log_debug("identity error", err);
  std::error_code expected = std::make_error_code(std::errc::invalid_argument);
  EXPECT_EQ(expected, err);
}

TEST(identity, cert_with_valid_version) {
  std::error_code err;
  sys::Identity identity{kGarbageCertificate,
                         kGarbagePrivateKey,
                         kGarbageCertificate,
                         "TLS1.1-tls1.2",
                         "",
                         "",
                         err};

  asio::error_code expected;
  EXPECT_EQ(expected, err);
  EXPECT_EQ(SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
            identity.peerVerifyMode());

  // EXPECT_EQ(identity.minProtoVersion(), TLS1_1_VERSION);
  // EXPECT_EQ(identity.maxProtoVersion(), TLS1_2_VERSION);
}
