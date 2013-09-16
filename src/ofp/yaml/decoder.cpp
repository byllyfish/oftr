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

namespace ofp {  // <namespace ofp>
namespace yaml { // <namespace yaml>

Decoder::Decoder(Message *msg) : msg_{msg}
{
    assert(msg->size() >= sizeof(Header));

    llvm::raw_string_ostream rss{result_};
    llvm::yaml::Output yout{rss};
    yout << *this;
    (void)rss.str();

    if (!error_.empty()) {
        result_.clear();
    }
}

template <class MsgType>
inline bool decode(llvm::yaml::IO &io, const Message *msg)
{
    const MsgType *m = MsgType::cast(msg);
    if (m == nullptr)
        return false;
    io.mapRequired("msg", RemoveConst_cast(*m));
    return true;
}

bool Decoder::decodeMsg(llvm::yaml::IO &io)
{
    switch (msg_->type()) {
    case Hello::type() :
        return decode<Hello>(io, msg_);
    case Error::type() :
        return decode<Error>(io, msg_);
    case EchoRequest::type() :
        return decode<EchoRequest>(io, msg_);
    case EchoReply::type() :
        return decode<EchoReply>(io, msg_);
    case Experimenter::type() :
        return decode<Experimenter>(io, msg_);
    case FlowMod::type() :
        return decode<FlowMod>(io, msg_);
    case FeaturesRequest::type() :
        return decode<FeaturesRequest>(io, msg_);
    case FeaturesReply::type() :
        return decode<FeaturesReply>(io, msg_);
    case GetConfigRequest::type() :
        return decode<GetConfigRequest>(io, msg_);
    case MultipartRequest::type() :
        return decode<MultipartRequest>(io, msg_);
    case MultipartReply::type() :
        return decode<MultipartReply>(io, msg_);
    case BarrierRequest::type() :
        return decode<BarrierRequest>(io, msg_);
    case BarrierReply::type() :
        return decode<BarrierReply>(io, msg_);
    case GetAsyncRequest::type() :
        return decode<GetAsyncRequest>(io, msg_);
    case PacketIn::type():
        return decode<PacketIn>(io, msg_);
    case PacketOut::type():
        return decode<PacketOut>(io, msg_);
    case SetConfig::type():
        return decode<SetConfig>(io, msg_);
    default:
        log::info("Decoder::decodeMsg: Unknown msg type", msg_->type());
        break;
    }

    return false;
}

} // </namespace yaml>
} // </namespace ofp>