#include "ofp/yaml/encoder.h"
#include "ofp/yaml/yflowmod.h"

namespace ofp {  // <namespace ofp>
namespace yaml { // <namespace yaml>

void Encoder::diagnosticHandler(const llvm::SMDiagnostic &diag, void *context)
{
    Encoder *encoder = reinterpret_cast<Encoder *>(context);
    encoder->addDiagnostic(diag);
}

void Encoder::encodeMsg(llvm::yaml::IO &io, Header &header)
{
    log::debug("Encoder::encodeMsg");

    switch (header.type()) {
    case FlowMod::Type: {
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