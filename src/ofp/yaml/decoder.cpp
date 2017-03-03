// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/yaml/decoder.h"
#include "ofp/requestforward.h"
#include "ofp/yaml/outputjson.h"
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
#include "ofp/yaml/yrolereply.h"
#include "ofp/yaml/yrolerequest.h"
#include "ofp/yaml/yrolestatus.h"
#include "ofp/yaml/ysetasync.h"
#include "ofp/yaml/ysetconfig.h"
#include "ofp/yaml/ytablemod.h"
#include "ofp/yaml/ytablestatus.h"

using namespace ofp;
using namespace ofp::yaml;

Decoder::Decoder(const Message *msg, bool useJsonFormat, bool includePktMatch)
    : msg_{msg} {
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

/// \brief Private constructor used when we need to recursively decode an
/// inner OpenFlow message, like in RequestForward messages.
Decoder::Decoder(const Message *msg, const Decoder *decoder) : msg_{msg} {
  assert(msg->size() >= sizeof(Header));
}

template <class MsgType>
inline bool decode(llvm::yaml::IO &io, const Message *msg) {
  const MsgType *m = MsgType::cast(msg);
  if (m == nullptr)
    return false;
  io.mapRequired("msg", RemoveConst_cast(*m));
  return true;
}

template <class MsgType>
inline bool decodeMultipart(llvm::yaml::IO &io, const Message *msg) {
  const MsgType *m = MsgType::cast(msg);
  if (m == nullptr)
    return false;
  llvm::yaml::MappingTraits<MsgType>::decode(io, RemoveConst_cast(*m),
                                             msg->subtype(), "msg");
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
      return decodeMultipart<MultipartRequest>(io, msg_);
    case MultipartReply::type():
      return decodeMultipart<MultipartReply>(io, msg_);
    case BarrierRequest::type():
      return decode<BarrierRequest>(io, msg_);
    case BarrierReply::type():
      return decode<BarrierReply>(io, msg_);
    case GetAsyncRequest::type():
      return decode<GetAsyncRequest>(io, msg_);
    case PacketIn::type():
      return decode<PacketIn>(io, msg_);
    case PacketOut::type():
      if (msg_->version() < OFP_VERSION_6) {
        return decode<PacketOut>(io, msg_);
      } else {
        return decode<PacketOutV6>(io, msg_);
      }
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
    case RoleStatus::type():
      return decode<RoleStatus>(io, msg_);
    case BundleControl::type():
      return decode<BundleControl>(io, msg_);
    case BundleAddMessage::type():
      return decode<BundleAddMessage>(io, msg_);
    case RequestForward::type():
      return decodeRequestForward(io, msg_);
    case TableStatus::type():
      return decode<TableStatus>(io, msg_);
    default:
      OFPErrorCode error;
      Validation context{msg_, &error};
      context.messageTypeIsNotSupported();
      break;
  }

  return false;
}

bool Decoder::decodeRequestForward(llvm::yaml::IO &io, const Message *msg) {
  const RequestForward *m = RequestForward::cast(msg);
  if (m == nullptr)
    return false;

  auto data = m->request();
  Message message(data.data(), data.size());
  message.setInfo(msg->info());
  message.normalize();

  DecodeRecursively(io, "msg", &message);
  return true;
}

void ofp::yaml::DecodeRecursively(llvm::yaml::IO &io, const char *key,
                                  const Message *msg) {
  Decoder *outerDecoder = GetDecoderFromContext(io);
  Decoder decoder{msg, outerDecoder};
  io.mapRequired(key, decoder);

  outerDecoder->setError(decoder.error());
}
