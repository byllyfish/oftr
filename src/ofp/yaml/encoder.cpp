// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/yaml/encoder.h"
#include "ofp/requestforward.h"
#include "ofp/yaml/ybundleaddmessage.h"
#include "ofp/yaml/ybundlecontrol.h"
#include "ofp/yaml/yecho.h"
#include "ofp/yaml/yerror.h"
#include "ofp/yaml/yexperimenter.h"
#include "ofp/yaml/yfeaturesreply.h"
#include "ofp/yaml/yflowmod.h"
#include "ofp/yaml/yflowremoved.h"
#include "ofp/yaml/ygetasyncreply.h"
#include "ofp/yaml/ygetconfigreply.h"
#include "ofp/yaml/ygroupmod.h"
#include "ofp/yaml/yheaderonly.h"
#include "ofp/yaml/yhello.h"
#include "ofp/yaml/ymetermod.h"
#include "ofp/yaml/ymultipartreply.h"
#include "ofp/yaml/ymultipartrequest.h"
#include "ofp/yaml/ypacketin.h"
#include "ofp/yaml/ypacketout.h"
#include "ofp/yaml/yportmod.h"
#include "ofp/yaml/yportstatus.h"
#include "ofp/yaml/yqueuegetconfigreply.h"
#include "ofp/yaml/yqueuegetconfigrequest.h"
#include "ofp/yaml/yrawmessage.h"
#include "ofp/yaml/yrolereply.h"
#include "ofp/yaml/yrolerequest.h"
#include "ofp/yaml/yrolestatus.h"
#include "ofp/yaml/ysetasync.h"
#include "ofp/yaml/ysetconfig.h"
#include "ofp/yaml/ytablemod.h"
#include "ofp/yaml/ytablestatus.h"

using namespace ofp;
using namespace ofp::yaml;

Encoder::Encoder(ChannelFinder finder)
    : errorStream_{error_},
      header_{OFPT_UNSUPPORTED},
      finder_{finder},
      defaultVersion_{0},
      matchPrereqsChecked_{true} {}

/// \brief Private constructor used when an encoder needs to encode a message
/// recursively.
Encoder::Encoder(const Encoder *encoder)
    : errorStream_{error_},
      header_{encoder->header_},
      finder_{nullptr},
      outputChannel_{encoder->outputChannel_},
      defaultVersion_{encoder->defaultVersion_},
      matchPrereqsChecked_{encoder->matchPrereqsChecked_} {}

Encoder::Encoder(const std::string &input, bool matchPrereqsChecked,
                 int lineNumber, UInt8 defaultVersion, ChannelFinder finder)
    : errorStream_{error_},
      header_{OFPT_UNSUPPORTED},
      finder_{finder},
      lineNumber_{lineNumber},
      defaultVersion_{defaultVersion},
      matchPrereqsChecked_{matchPrereqsChecked} {
  llvm::yaml::Input yin{input, nullptr, Encoder::diagnosticHandler, this};
  detail::YamlContext ctxt{this, &yin};
  yin.setContext(&ctxt);
  if (!yin.error()) {
    yin >> *this;
  }
  yin.setContext(nullptr);

  if (yin.error()) {
    // Make sure error string is set. There won't be an error string if the
    // document is empty.
    if (error().empty()) {
      errorStream_ << "YAML:" << lineNumber_ << ":1: error: not a document";
    }
    channel_.clear();

  } else if (channel_.size() < sizeof(Header)) {
    // If the message isn't implemented, no output will be produced and there
    // will be no error.
    errorStream_
        << "YAML:" << lineNumber_
        << ":1: error: no output produced; check implementation status";
  }

  assert(channel_.size() >= sizeof(Header) || !error().empty());
}

void Encoder::diagnosticHandler(const llvm::SMDiagnostic &d, void *context) {
  Encoder *encoder = reinterpret_cast<Encoder *>(context);
  encoder->addDiagnostic(d);
}

void Encoder::addDiagnostic(const llvm::SMDiagnostic &d) {
  llvm::SMDiagnostic diag{*d.getSourceMgr(), d.getLoc(),
                          d.getFilename(),   d.getLineNo() + lineNumber_ - 1,
                          d.getColumnNo(),   d.getKind(),
                          d.getMessage(),    d.getLineContents(),
                          d.getRanges()};
  diag.print("", errorStream_, false);
}

void Encoder::encodeMsg(llvm::yaml::IO &io) {
  // At this point, we know the datapathID. The YAML message may not contain
  // the version, and even if it does, we still need to override the
  // version if the channel corresponds to an actual channel. Use the channel
  // finder to locate the channel for the given datapathID so we can set the
  // correct protocol version.

  if (finder_) {
    // If there's a datapath or connId specified, look up the channel.
    outputChannel_ = finder_(connId_, datapathId_);
    if (!outputChannel_) {
      if (connId_ != 0) {
        io.setError("unable to locate conn_id " + std::to_string(connId_));
      } else if (!datapathId_.empty()) {
        io.setError("unable to locate datapath_id " + datapathId_.toString());
      } else {
        io.setError("unable to locate connection; no conn_id or datapath_id");
      }
      return;
    }
    // Channel version will override any version specified by input.
    if (!header_.version()) {
      header_.setVersion(outputChannel_->version());
    } else if (header_.version() != outputChannel_->version()) {
      log_warning("Message version", header_.version(),
                  "does not match channel version", outputChannel_->version());
    }

  } else if (!header_.version()) {
    // If version is unset, set it to the default version. N.B. The `Hello`
    // message type can infer its version number from its contents.
    if (!defaultVersion_ && header_.type() != Hello::type()) {
      io.setError("unspecified protocol version");
      return;
    }
    header_.setVersion(defaultVersion_);
  }

  // Make sure the message type is supported for this version number.
  if (!header_.validateVersionAndType()) {
    io.setError("invalid combination of version and type");
    return;
  }

  // Set the version and xid for the memory channel.
  channel_.setVersion(header_.version());
  channel_.setNextXid(header_.xid());

  switch (header_.type()) {
    case Hello::type(): {
      HelloBuilder hello{header_.version()};
      io.mapOptional("msg", hello);
      hello.send(&channel_);
      break;
    }
    case Error::type(): {
      ErrorBuilder error{header_.xid()};
      io.mapRequired("msg", error);
      error.send(&channel_);
      break;
    }
    case EchoRequest::type(): {
      EchoRequestBuilder echo;
      io.mapOptional("msg", echo);
      echo.send(&channel_);
      break;
    }
    case EchoReply::type(): {
      EchoReplyBuilder echo{header_.xid()};
      io.mapOptional("msg", echo);
      echo.send(&channel_);
      break;
    }
    case Experimenter::type(): {
      ExperimenterBuilder experimenter;
      io.mapRequired("msg", experimenter);
      experimenter.send(&channel_);
      break;
    }
    case FeaturesRequest::type(): {
      FeaturesRequestBuilder features;
      io.mapOptional("msg", features);
      features.send(&channel_);
      break;
    }
    case FeaturesReply::type(): {
      FeaturesReplyBuilder features{header_.xid()};
      io.mapRequired("msg", features);
      features.send(&channel_);
      break;
    }
    case GetConfigRequest::type(): {
      GetConfigRequestBuilder config;
      io.mapOptional("msg", config);
      config.send(&channel_);
      break;
    }
    case GetConfigReply::type(): {
      GetConfigReplyBuilder config;
      io.mapRequired("msg", config);
      config.send(&channel_);
      break;
    }
    case FlowMod::type(): {
      FlowModBuilder flowMod;
      io.mapRequired("msg", flowMod);
      flowMod.send(&channel_);
      break;
    }
    case BarrierRequest::type(): {
      BarrierRequestBuilder barrier;
      io.mapOptional("msg", barrier);
      barrier.send(&channel_);
      break;
    }
    case BarrierReply::type(): {
      BarrierReplyBuilder barrier;
      io.mapOptional("msg", barrier);
      barrier.send(&channel_);
      break;
    }
    case GetAsyncRequest::type(): {
      GetAsyncRequestBuilder async;
      io.mapOptional("msg", async);
      async.send(&channel_);
      break;
    }
    case MultipartRequest::type(): {
      MultipartRequestBuilder multi{channel_.version()};
      if (subtype_ != OFPMP_UNSUPPORTED) {
        multi.setRequestType(subtype_);
        multi.setRequestFlags(toMultipartFlags(flags_));
        llvm::yaml::MappingTraits<MultipartRequestBuilder>::encode(
            io, multi, subtype_, "msg");
      } else {
        // This supports the older YAML input format for MultipartRequest.
        io.mapRequired("msg", multi);
      }
      multi.send(&channel_);
      break;
    }
    case MultipartReply::type(): {
      MultipartReplyBuilder multi{channel_.version()};
      if (subtype_ != OFPMP_UNSUPPORTED) {
        multi.setReplyType(subtype_);
        multi.setReplyFlags(toMultipartFlags(flags_));
        llvm::yaml::MappingTraits<MultipartReplyBuilder>::encode(
            io, multi, subtype_, "msg");
      } else {
        // This supports the older YAML input format for MultipartReply.
        io.mapRequired("msg", multi);
      }
      multi.send(&channel_);
      break;
    }
    case PacketIn::type(): {
      PacketInBuilder packetIn;
      io.mapRequired("msg", packetIn);
      packetIn.send(&channel_);
      break;
    }
    case PacketOut::type(): {
      if (header_.version() < OFP_VERSION_6) {
        PacketOutBuilder packetOut;
        io.mapRequired("msg", packetOut);
        packetOut.send(&channel_);
      } else {
        PacketOutV6Builder packetOutV6;
        io.mapRequired("msg", packetOutV6);
        packetOutV6.send(&channel_);
      }
      break;
    }
    case SetConfig::type(): {
      SetConfigBuilder setConfig;
      io.mapRequired("msg", setConfig);
      setConfig.send(&channel_);
      break;
    }
    case PortStatus::type(): {
      PortStatusBuilder portStatus;
      io.mapRequired("msg", portStatus);
      portStatus.send(&channel_);
      break;
    }
    case GroupMod::type(): {
      GroupModBuilder groupMod;
      io.mapRequired("msg", groupMod);
      groupMod.send(&channel_);
      break;
    }
    case PortMod::type(): {
      PortModBuilder portMod;
      io.mapRequired("msg", portMod);
      portMod.send(&channel_);
      break;
    }
    case TableMod::type(): {
      TableModBuilder tableMod;
      io.mapRequired("msg", tableMod);
      tableMod.send(&channel_);
      break;
    }
    case RoleRequest::type(): {
      RoleRequestBuilder roleReq;
      io.mapRequired("msg", roleReq);
      roleReq.send(&channel_);
      break;
    }
    case RoleReply::type(): {
      RoleReplyBuilder roleReply{header_.xid()};
      io.mapRequired("msg", roleReply);
      roleReply.send(&channel_);
      break;
    }
    case GetAsyncReply::type(): {
      GetAsyncReplyBuilder asyncReply{header_.xid()};
      io.mapRequired("msg", asyncReply);
      asyncReply.send(&channel_);
      break;
    }
    case SetAsync::type(): {
      SetAsyncBuilder setAsync;
      io.mapRequired("msg", setAsync);
      setAsync.send(&channel_);
      break;
    }
    case QueueGetConfigRequest::type(): {
      QueueGetConfigRequestBuilder queueGetConfig;
      io.mapRequired("msg", queueGetConfig);
      queueGetConfig.send(&channel_);
      break;
    }
    case QueueGetConfigReply::type(): {
      QueueGetConfigReplyBuilder queueGetConfigReply;
      io.mapRequired("msg", queueGetConfigReply);
      queueGetConfigReply.send(&channel_);
      break;
    }
    case FlowRemoved::type(): {
      if (header_.version() < OFP_VERSION_6) {
        FlowRemovedBuilder flowRemoved;
        io.mapRequired("msg", flowRemoved);
        flowRemoved.send(&channel_);
      } else {
        FlowRemovedV6Builder flowRemoved;
        io.mapRequired("msg", flowRemoved);
        flowRemoved.send(&channel_);
      }
      break;
    }
    case MeterMod::type(): {
      MeterModBuilder meterMod;
      io.mapRequired("msg", meterMod);
      meterMod.send(&channel_);
      break;
    }
    case RoleStatus::type(): {
      RoleStatusBuilder roleStatus;
      io.mapRequired("msg", roleStatus);
      roleStatus.send(&channel_);
      break;
    }
    case BundleControl::type(): {
      BundleControlBuilder bundleControl;
      io.mapRequired("msg", bundleControl);
      bundleControl.send(&channel_);
      break;
    }
    case BundleAddMessage::type(): {
      BundleAddMessageBuilder addMessage;
      io.mapRequired("msg", addMessage);
      addMessage.send(&channel_);
      break;
    }
    case RequestForward::type(): {
      ByteList data;
      EncodeRecursively(io, "msg", data);
      RequestForwardBuilder reqForward;
      reqForward.setRequest(data);
      reqForward.send(&channel_);
      break;
    }
    case TableStatus::type(): {
      TableStatusBuilder tableStatus;
      io.mapRequired("msg", tableStatus);
      tableStatus.send(&channel_);
      break;
    }
    case OFPT_RAW_MESSAGE: {
      RawMessageBuilder rawMessage{header_.xid()};
      io.mapRequired("msg", rawMessage);
      rawMessage.send(&channel_);
      break;
    }
    default:
      log_info("yaml::Encoder::encodeMsg: Unsupported message type:",
               static_cast<int>(header_.type()));
      break;
  }
}

void ofp::yaml::EncodeRecursively(llvm::yaml::IO &io, const char *key,
                                  ByteList &data) {
  auto parent = GetEncoderFromContext(io);
  log::fatal_if_null(parent, "EncodeRecursively: missing context");

  parent->setRecursive(true);
  Encoder encoder{parent};
  io.mapRequired(key, encoder);
  data.set(encoder.data(), encoder.size());
  parent->setRecursive(false);
}
