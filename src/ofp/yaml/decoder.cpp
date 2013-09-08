#include "ofp/yaml/decoder.h"
#include "ofp/yaml/yhello.h"
#include "ofp/yaml/yflowmod.h"

namespace ofp {  // <namespace ofp>
namespace yaml { // <namespace yaml>

Decoder::Decoder(Message *msg) : msg_{msg}
{
    assert(msg->size() >= sizeof(Header));

    llvm::raw_string_ostream rss{result_};
    llvm::yaml::Output yout{rss};
    yout << *this;
    (void)rss.str();
}

void Decoder::decodeMsg(llvm::yaml::IO &io)
{
    switch (msg_->type()) {
    case Hello::type() : {
        const Hello *hello = Hello::cast(msg_);
        if (!hello)
            goto ERROR;
        io.mapRequired("msg", detail::YamlRemoveConst_cast(*hello));
        break;
    }
    case FlowMod::type() : {
        const FlowMod *flowMod = FlowMod::cast(msg_);
        if (!flowMod)
            goto ERROR;
        io.mapRequired("msg", detail::YamlRemoveConst_cast(*flowMod));
        break;
    }
    default:
        break;
    }

    return;

ERROR:
    error_ = "Invalid packet:";
}

} // </namespace yaml>
} // </namespace ofp>