// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YCONTEXT_H_
#define OFP_YAML_YCONTEXT_H_

#include "ofp/types.h"

namespace ofp {
namespace yaml {

class Encoder;
class Decoder;

namespace detail {

// A ptr to the YamlContext is passed as the IO's context for YAML operations.
// The YamlContext provides access to the current encoder, decoder and protocol
// version. The protocol version is only available when decoding a binary
// OpenFlow packet.
//
// When decoding a message, the `pktMatch` option specifies whether to include
// the `data_match` field when decoding a PacketIn message.

struct YamlContext {
  explicit YamlContext(Encoder *enc)
      : encoder{enc}, decoder{nullptr}, version{0}, pktMatch{false} {
    assert(validate());
  }
  explicit YamlContext(Decoder *dec, UInt8 ver, bool pMatch)
      : encoder{nullptr}, decoder{dec}, version{ver}, pktMatch{pMatch} {
    assert(validate());
  }

  Encoder *encoder;
  Decoder *decoder;
  UInt8 version;
  bool pktMatch;

  // Use magic values to verify that context exists and is valid.
  UInt8 magic1 = 'c';
  UInt8 magic2 = 'o';
  UInt8 padding[4];

  bool validate() const { return magic1 == 'c' && magic2 == 'o'; }

  static Encoder *GetEncoder(void *context);
};

}  // namespace detail

Encoder *GetEncoderFromContext(llvm::yaml::IO &io);
Decoder *GetDecoderFromContext(llvm::yaml::IO &io);
UInt8 GetVersionFromContext(llvm::yaml::IO &io);
bool GetIncludePktMatchFromContext(llvm::yaml::IO &io);

}  // namespace yaml
}  // namespace ofp

#endif  // OFP_YAML_YCONTEXT_H_
