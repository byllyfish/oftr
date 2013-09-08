#include "ofp/yaml/encoder.h"
#include "ofp/yaml/yhello.h"
#include "ofp/yaml/yerror.h"
#include "ofp/yaml/yflowmod.h"

namespace ofp {  // <namespace ofp>
namespace yaml { // <namespace yaml>


Encoder::Encoder(const std::string &input) : errorStream_{error_} 
{
    llvm::yaml::Input yin{input, nullptr, Encoder::diagnosticHandler, this};
    if (!yin.error()) {
        yin >> *this;
    }
}

void Encoder::diagnosticHandler(const llvm::SMDiagnostic &diag, void *context)
{
    Encoder *encoder = reinterpret_cast<Encoder *>(context);
    encoder->addDiagnostic(diag);
}

void Encoder::encodeMsg(llvm::yaml::IO &io, Header &header)
{
    log::debug("Encoder::encodeMsg");
    channel_.setVersion(header.version());
    channel_.setNextXid(header.xid());

    switch (header.type()) {
    case Hello::type(): {
        HelloBuilder hello{header};
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
    case FlowMod::type(): {
        FlowModBuilder flowMod;
        io.mapRequired("msg", flowMod);
        flowMod.send(&channel_);
        break;
    }
    default:
        break;
    }
}

} // </namespace yaml>
} // </namespace ofp>