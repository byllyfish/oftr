// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_API_APIENCODER_H_
#define OFP_API_APIENCODER_H_

#include "ofp/yaml/yllvm.h"
#include "ofp/api/apievents.h"

namespace ofp {
namespace api {

class ApiConnection;

/// Parse YAML API events and pass them to the connection as event structures.
/// If there is an error parsing the YAML, write an OFPLIB_PROTOCOL_ERROR event
/// back to the connection. This class is called an 'encoder' since it
/// translates from YAML to an internal binary representation.

class ApiEncoder {
 public:
  ApiEncoder(const std::string &input, ApiConnection *conn);

 private:
  ApiConnection *conn_;
  std::string error_;
  llvm::raw_string_ostream errorStream_;

  static void diagnosticHandler(const llvm::SMDiagnostic &diag, void *context);

  void addDiagnostic(const llvm::SMDiagnostic &diag) {
    diag.print("", errorStream_, false);
  }

  void encodeMsg(llvm::yaml::IO &io, ApiEvent event);  // NOLINT

  friend struct llvm::yaml::MappingTraits<ofp::api::ApiEncoder>;
};

}  // namespace api
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::api::ApiEncoder> {
  static void mapping(IO &io, ofp::api::ApiEncoder &encoder) {  // NOLINT
    ofp::api::ApiEvent event = ofp::api::LIBOFP_INVALID;
    io.mapRequired("event", event);
    encoder.encodeMsg(io, event);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_API_APIENCODER_H_
