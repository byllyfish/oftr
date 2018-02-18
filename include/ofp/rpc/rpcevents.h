// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_RPC_RPCEVENTS_H_
#define OFP_RPC_RPCEVENTS_H_

#include "ofp/datapathid.h"
#include "ofp/driver.h"
#include "ofp/padding.h"
#include "ofp/rpc/filteractiongenericreply.h"
#include "ofp/rpc/filtertableentry.h"
#include "ofp/rpc/rpcid.h"
#include "ofp/yaml/encoder.h"

namespace ofp {
namespace rpc {

/// The maximum RPC message size is 1MB.
const size_t RPC_MAX_MESSAGE_SIZE = 1048576;

/// RPC events are delimited by zero byte (in the text protocol).
const char RPC_EVENT_DELIMITER_CHAR = '\x00';

/// RPC event tag byte (in the binary protocol).
const UInt8 RPC_EVENT_BINARY_TAG = 0xF5;

/// RPC Methods
enum RpcMethod : UInt32 {
  METHOD_LISTEN = 0,    // OFP.LISTEN
  METHOD_CONNECT,       // OFP.CONNECT
  METHOD_CLOSE,         // OFP.CLOSE
  METHOD_SEND,          // OFP.SEND
  METHOD_MESSAGE,       // OFP.MESSAGE
  METHOD_LIST_CONNS,    // OFP.LIST_CONNECTIONS
  METHOD_ADD_IDENTITY,  // OFP.ADD_IDENTITY
  METHOD_DESCRIPTION,   // OFP.DESCRIPTION
  METHOD_SET_FILTER,    // OFP.SET_FILTER
  METHOD_UNSUPPORTED
};

/// JSON-RPC error codes.
enum RpcErrorCode {
  ERROR_CODE_INVALID_REQUEST = -32600,
  ERROR_CODE_METHOD_NOT_FOUND = -32601,
  ERROR_CODE_CONNECTION_NOT_FOUND = -65000,
  ERROR_CODE_TLS_ID_NOT_FOUND = -65001,
  ERROR_CODE_INVALID_OPTIONS = -65002,
};

OFP_BEGIN_IGNORE_PADDING

/// Represents the general JSON-RPC error response.
struct RpcErrorResponse {
  explicit RpcErrorResponse(RpcID ident) : id{ident} {}
  std::string toJson();

  struct Error {
    SignedInt32 code = 0;
    std::string message;
  };

  RpcID id;
  Error error;
};

/// Represents an object in a ofp.list_connections result list.
struct RpcConnectionStats {
  IPv6Endpoint localEndpoint;
  IPv6Endpoint remoteEndpoint;
  UInt64 connId;
  DatapathID datapathId;
  UInt8 auxiliaryId;
  ChannelTransport transport;
};

/// Represents a RPC request to describe the RPC server (METHOD_DESCRIPTION)
struct RpcDescription {
  explicit RpcDescription(RpcID ident) : id{ident} {}

  struct Params {};

  RpcID id;
  Params params;
};

/// Represents a RPC response to describe the RPC server (METHOD_DESCRIPTION)
struct RpcDescriptionResponse {
  explicit RpcDescriptionResponse(RpcID ident) : id{ident} {}
  std::string toJson();

  struct Result {
    /// Current API version.
    std::string api_version;
    /// Current version of this software.
    std::string sw_desc;
    /// List of supported OpenFlow versions.
    std::vector<UInt8> versions;
  };

  RpcID id;
  Result result;
};

/// Represents a RPC request to listen for new connections (METHOD_LISTEN)
struct RpcListen {
  explicit RpcListen(RpcID ident) : id{ident} {}

  struct Params {
    /// Endpoint to listen on.
    IPv6Endpoint endpoint;
    /// Optional list of desired versions (empty means all).
    std::vector<UInt8> versions;
    /// Optional TLS identity.
    UInt64 securityId = 0;
    /// Array of options.
    std::vector<std::string> options;
  };

  RpcID id;
  Params params;
};

/// Represents a RPC response to listen for new connections (METHOD_LISTEN)
struct RpcListenResponse {
  explicit RpcListenResponse(RpcID ident) : id{ident} {}
  std::string toJson();

  struct Result {
    /// Connection ID of listening connection.
    UInt64 connId = 0;
  };

  RpcID id;
  Result result;
};

/// Represents a RPC request to make an outgoing connection (METHOD_CONNECT)
struct RpcConnect {
  explicit RpcConnect(RpcID ident) : id{ident} {}

  struct Params {
    /// Endpoint to connect to.
    IPv6Endpoint endpoint;
    /// Optional list of desired versions (empty means all).
    std::vector<UInt8> versions;
    /// Optional TLS identity.
    UInt64 securityId = 0;
    /// Array of options.
    std::vector<std::string> options;
  };

  RpcID id;
  Params params;
};

/// Represents a RPC response to make an outgoing connection (METHOD_CONNECT)
struct RpcConnectResponse {
  explicit RpcConnectResponse(RpcID ident) : id{ident} {}
  std::string toJson();

  struct Result {
    /// Connection ID of outgoing connection.
    UInt64 connId = 0;
  };

  RpcID id;
  Result result;
};

/// Represents a RPC request to close a connection (METHOD_CLOSE)
struct RpcClose {
  explicit RpcClose(RpcID ident) : id{ident} {}

  struct Params {
    /// Connection ID to close.
    UInt64 connId = 0;
    /// DatapathID to close.
    DatapathID datapathId;
  };

  RpcID id;
  Params params;
};

/// Represents a RPC response to close a connection (METHOD_CLOSE)
struct RpcCloseResponse {
  explicit RpcCloseResponse(RpcID ident) : id{ident} {}
  std::string toJson();

  struct Result {
    /// Count of closed connections.
    UInt32 count = 0;
  };

  RpcID id;
  Result result;
};

/// Represents a RPC request to list connection stats (METHOD_LIST_CONNS)
struct RpcListConns {
  explicit RpcListConns(RpcID ident) : id{ident} {}

  struct Params {
    UInt64 connId = 0;
  };

  RpcID id;
  Params params;
};

struct RpcListConnsResponse {
  explicit RpcListConnsResponse(RpcID ident) : id{ident} {}
  std::string toJson();

  struct Result {
    /// List of connections.
    std::vector<RpcConnectionStats> stats;
  };

  RpcID id;
  Result result;
};

/// Represents a RPC request to add an identity (METHOD_ADD_IDENTITY)
struct RpcAddIdentity {
  explicit RpcAddIdentity(RpcID ident) : id{ident} {}

  struct Params {
    /// Certificate chain in PEM format.
    std::string cert;
    /// Private key for certificate in `cert`.
    std::string privkey;
    /// PEM certificate for trusted CA to use for verifying a peer certificate.
    std::string cacert;
    /// TLS Protocol version.
    std::string version;
    /// Supported ciphers.
    std::string ciphers;
    /// Key log file.
    std::string keylog;
  };

  RpcID id;
  Params params;
};

/// Represents a RPC response to add an identity (METHOD_ADD_IDENTITY)
struct RpcAddIdentityResponse {
  explicit RpcAddIdentityResponse(RpcID ident) : id{ident} {}
  std::string toJson();

  struct Result {
    /// Security ID of identity added.
    UInt64 securityId = 0;
  };

  RpcID id;
  Result result;
};

/// Represents a RPC request to send a message to datapath (METHOD_SEND).
struct RpcSend {
  explicit RpcSend(RpcID ident, yaml::Encoder::ChannelFinder finder)
      : id{ident}, params{finder} {}

  RpcID id;
  yaml::Encoder params;
};

/// Represnts a RPC response to send a message to datapath (METHOD_SEND).
struct RpcSendResponse {
  explicit RpcSendResponse(RpcID ident) : id{ident} {}
  std::string toJson();

  struct Result {
    ByteRange data;  // header of message sent
  };

  RpcID id;
  Result result;
};

/// Represents a RPC request to set a packet filter (METHOD_SET_FILTER).
struct RpcSetFilter {
  explicit RpcSetFilter(RpcID ident) : id{ident} {}

  RpcID id;
  std::vector<FilterTableEntry> params;
};

/// Represents a RPC response to set packet filter (METHOD_SET_FILTER).
struct RpcSetFilterResponse {
  explicit RpcSetFilterResponse(RpcID ident) : id{ident} {}
  std::string toJson();

  struct Result {
    /// Filter count.
    UInt32 count = 0;
  };

  RpcID id;
  Result result;
};

/// Represents a RPC notification about a channel (METHOD_MESSAGE subtype)
struct RpcChannel {
  std::string toJson();

  struct Params {
    std::string type;
    Timestamp time;
    UInt64 connId = 0;
    DatapathID datapathId;
    IPv6Endpoint endpoint;
    UInt8 version{};
  };

  Params params;
};

/// Represents a RPC notification about some alert (e.g. malformed message)
/// (METHOD_MESSAGE subtype).
struct RpcAlert {
  std::string toJson();

  struct Params {
    std::string type;
    Timestamp time;
    UInt64 connId = 0;
    DatapathID datapathId;
    UInt32 xid = 0;
    std::string alert;
    ByteRange data;
  };

  Params params;
};

OFP_END_IGNORE_PADDING

}  // namespace rpc
}  // namespace ofp

#endif  // OFP_RPC_RPCEVENTS_H_
