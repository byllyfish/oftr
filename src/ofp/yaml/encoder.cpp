// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/yaml/encoder.h"
#include "ofp/yaml/yhello.h"
#include "ofp/yaml/yerror.h"
#include "ofp/yaml/yflowmod.h"
#include "ofp/yaml/yecho.h"
#include "ofp/yaml/yexperimenter.h"
#include "ofp/yaml/yheaderonly.h"
#include "ofp/yaml/yfeaturesreply.h"
#include "ofp/yaml/ymultipartrequest.h"
#include "ofp/yaml/ymultipartreply.h"
#include "ofp/yaml/ypacketin.h"
#include "ofp/yaml/ypacketout.h"
#include "ofp/yaml/ysetconfig.h"
#include "ofp/yaml/yportstatus.h"
#include "ofp/yaml/ygroupmod.h"
#include "ofp/yaml/yportmod.h"
#include "ofp/yaml/ytablemod.h"
#include "ofp/yaml/yrolerequest.h"
#include "ofp/yaml/yrolereply.h"
#include "ofp/yaml/ygetasyncreply.h"
#include "ofp/yaml/yqueuegetconfigrequest.h"
#include "ofp/yaml/yqueuegetconfigreply.h"
#include "ofp/yaml/ygetconfigreply.h"
#include "ofp/yaml/ysetasync.h"
#include "ofp/yaml/yflowremoved.h"
#include "ofp/yaml/ymetermod.h"

namespace ofp {
namespace yaml {

Encoder::Encoder(ChannelFinder finder)
    : errorStream_{error_},
      header_{OFPT_UNSUPPORTED},
      finder_{finder},
      matchPrereqsChecked_{true} {
}

Encoder::Encoder(const std::string &input, bool matchPrereqsChecked,
                 int lineNumber, ChannelFinder finder)
    : errorStream_{error_},
      header_{OFPT_UNSUPPORTED},
      finder_{finder},
      lineNumber_{lineNumber},
      matchPrereqsChecked_{matchPrereqsChecked} {
  detail::YamlContext ctxt{this};
  llvm::yaml::Input yin{input, &ctxt, Encoder::diagnosticHandler, &ctxt};
  if (!yin.error()) {
    yin >> *this;
  }

  if (yin.error()) {
    // Make sure error string is set. There won't be an error string if the
    // document is empty.
    if (error().empty()) {
      errorStream_ << "YAML:" << (lineNumber_ + 1)
                   << ":1: error: not a document";
    }
    channel_.clear();

  } else if (channel_.size() < sizeof(Header)) {
    // If the message isn't implemented, no output will be produced and there
    // will be no error.
    errorStream_
        << "YAML:" << (lineNumber_ + 1)
        << ":1: error: no output produced; check implementation status";
  }

  assert(channel_.size() >= sizeof(Header) || !error().empty());
}

void Encoder::diagnosticHandler(const llvm::SMDiagnostic &d, void *context) {
  Encoder *encoder = detail::YamlContext::GetEncoder(context);
  encoder->addDiagnostic(d);
}

void Encoder::addDiagnostic(const llvm::SMDiagnostic &d) {
  llvm::SMDiagnostic diag{*d.getSourceMgr(), d.getLoc(), d.getFilename(),
                          d.getLineNo() + lineNumber_, d.getColumnNo(),
                          d.getKind(), d.getMessage(), d.getLineContents(),
                          d.getRanges()};
  diag.print("", errorStream_, false);
}

void Encoder::encodeMsg(llvm::yaml::IO &io) {
  // At this point, we know the datapathID. The YAML message may not contain
  // the version and xid, and even if it does, we still need to override the
  // version if the channel corresponds to an actual channel. Use the channel
  // finder to locate the channel for the given datapathID so we can set the
  // correct protocol version and possibly override the xid.

  outputChannel_ = finder_(datapathId_, connId_);
  if (outputChannel_) {
    // Channel version will override any version specified by input.
    header_.setVersion(outputChannel_->version());
    if (header_.xid() == 0) {
      header_.setXid(outputChannel_->nextXid());
    }
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
      io.mapRequired("msg", multi);
      multi.send(&channel_);
      break;
    }
    case MultipartReply::type(): {
      MultipartReplyBuilder multi{channel_.version()};
      io.mapRequired("msg", multi);
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
      PacketOutBuilder packetOut;
      io.mapRequired("msg", packetOut);
      packetOut.send(&channel_);
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
      FlowRemovedBuilder flowRemoved;
      io.mapRequired("msg", flowRemoved);
      flowRemoved.send(&channel_);
      break;
    }
    case MeterMod::type(): {
      MeterModBuilder meterMod;
      io.mapRequired("msg", meterMod);
      meterMod.send(&channel_);
      break;
    }
    default:
      log::info("yaml::Encoder::encodeMsg: Unsupported message type:",
                static_cast<int>(header_.type()));
      break;
  }
}

Channel *Encoder::NullChannelFinder(const DatapathID &datapathId,
                                    UInt64 connId) {
  return nullptr;
}

}  // namespace yaml
}  // namespace ofp
