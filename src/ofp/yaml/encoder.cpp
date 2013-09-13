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

namespace ofp {  // <namespace ofp>
namespace yaml { // <namespace yaml>


Encoder::Encoder(const std::string &input) : errorStream_{error_} 
{
    if (input.empty()) {
        error_ = "No input.";
    } else {
        llvm::yaml::Input yin{input, nullptr, Encoder::diagnosticHandler, this};
        if (!yin.error()) {
            yin >> *this;
        }
        if (yin.error()) {
            channel_.clear();
        }
    }
}

void Encoder::diagnosticHandler(const llvm::SMDiagnostic &diag, void *context)
{
    Encoder *encoder = reinterpret_cast<Encoder *>(context);
    encoder->addDiagnostic(diag);
}


void Encoder::encodeMsg(llvm::yaml::IO &io, Header &header)
{
    channel_.setVersion(header.version());
    channel_.setNextXid(header.xid());

    switch (header.type()) {
    case Hello::type(): {
        HelloBuilder hello{header.version()};
        io.mapOptional("msg", hello);
        hello.send(&channel_);
        break;
    }
    case Error::type(): {
        ErrorBuilder error;
        io.mapOptional("msg", error);
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
        EchoReplyBuilder echo{header.xid()};
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
        FeaturesReplyBuilder features{header.xid()};
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
        MultipartRequestBuilder multi;
        io.mapRequired("msg", multi);
        multi.send(&channel_);
        break;
    }
    case MultipartReply::type(): {
        MultipartReplyBuilder multi;
        io.mapRequired("msg", multi);
        multi.send(&channel_);
        break;
    }
    default:
        break;
    }
}

} // </namespace yaml>
} // </namespace ofp>