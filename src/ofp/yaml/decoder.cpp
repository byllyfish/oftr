// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/yaml/decoder.h"
#include "ofp/yaml/yhello.h"
#include "ofp/yaml/yerror.h"
#include "ofp/yaml/yecho.h"
#include "ofp/yaml/yexperimenter.h"
#include "ofp/yaml/yflowmod.h"
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
#include "ofp/yaml/outputjson.h"

namespace ofp {
namespace yaml {

Decoder::Decoder(const Message *msg, bool useJsonFormat, bool includePktMatch) : msg_{msg} {
  assert(msg->size() >= sizeof(Header));

  llvm::raw_svector_ostream rss{result_};
  detail::YamlContext ctxt{this, msg->version(), includePktMatch};

  if (useJsonFormat) {
    ofp::yaml::OutputJson yout{rss, &ctxt};
    yout << *this;
  } else {
    llvm::yaml::Output yout{rss, &ctxt};
    yout << *this;
  }

  (void)rss.str();
  if (!error_.empty()) {
    result_.clear();
  }
}

template <class MsgType>
inline bool decode(llvm::yaml::IO &io, const Message *msg) {
  const MsgType *m = MsgType::cast(msg);
  if (m == nullptr) return false;
  io.mapRequired("msg", RemoveConst_cast(*m));
  return true;
}

bool Decoder::decodeMsg(llvm::yaml::IO &io) {
  switch (msg_->type()) {
    case Hello::type():
      return decode<Hello>(io, msg_);
    case Error::type():
      return decode<Error>(io, msg_);
    case EchoRequest::type():
      return decode<EchoRequest>(io, msg_);
    case EchoReply::type():
      return decode<EchoReply>(io, msg_);
    case Experimenter::type():
      return decode<Experimenter>(io, msg_);
    case FlowMod::type():
      return decode<FlowMod>(io, msg_);
    case FeaturesRequest::type():
      return decode<FeaturesRequest>(io, msg_);
    case FeaturesReply::type():
      return decode<FeaturesReply>(io, msg_);
    case GetConfigRequest::type():
      return decode<GetConfigRequest>(io, msg_);
    case GetConfigReply::type():
      return decode<GetConfigReply>(io, msg_);
    case MultipartRequest::type():
      return decode<MultipartRequest>(io, msg_);
    case MultipartReply::type():
      return decode<MultipartReply>(io, msg_);
    case BarrierRequest::type():
      return decode<BarrierRequest>(io, msg_);
    case BarrierReply::type():
      return decode<BarrierReply>(io, msg_);
    case GetAsyncRequest::type():
      return decode<GetAsyncRequest>(io, msg_);
    case PacketIn::type():
      return decode<PacketIn>(io, msg_);
    case PacketOut::type():
      return decode<PacketOut>(io, msg_);
    case SetConfig::type():
      return decode<SetConfig>(io, msg_);
    case PortStatus::type():
      return decode<PortStatus>(io, msg_);
    case GroupMod::type():
      return decode<GroupMod>(io, msg_);
    case PortMod::type():
      return decode<PortMod>(io, msg_);
    case TableMod::type():
      return decode<TableMod>(io, msg_);
    case RoleRequest::type():
      return decode<RoleRequest>(io, msg_);
    case RoleReply::type():
      return decode<RoleReply>(io, msg_);
    case GetAsyncReply::type():
      return decode<GetAsyncReply>(io, msg_);
    case SetAsync::type():
      return decode<SetAsync>(io, msg_);
    case QueueGetConfigRequest::type():
      return decode<QueueGetConfigRequest>(io, msg_);
    case QueueGetConfigReply::type():
      return decode<QueueGetConfigReply>(io, msg_);
    case FlowRemoved::type():
      return decode<FlowRemoved>(io, msg_);
    case MeterMod::type():
      return decode<MeterMod>(io, msg_);
    default:
      OFPErrorCode error;
      Validation context{msg_, &error};
      context.messageTypeIsNotSupported();
      break;
  }

  return false;
}

}  // namespace yaml
}  // namespace ofp
