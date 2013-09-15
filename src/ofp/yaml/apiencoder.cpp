#include "ofp/yaml/apiencoder.h"
#include "ofp/yaml/apiconnection.h"

using namespace ofp::yaml;

ApiEncoder::ApiEncoder(const std::string &input, ApiConnection *conn)
    : conn_{conn}, errorStream_{error_}
{
    ofp::log::debug("ApiEncoder: ", ofp::RawDataToHex(input.data(), input.length()));

    if (!input.empty()) {
        llvm::yaml::Input yin{input, nullptr,
                              ApiEncoder::diagnosticHandler, this};
        if (!yin.error()) {
            yin >> *this;
        }
        if (yin.error()) {
            conn_->onYamlError(errorStream_.str(), input);
        }
    }
}

void ApiEncoder::diagnosticHandler(const llvm::SMDiagnostic &diag,
                                   void *context)
{
    ApiEncoder *encoder = reinterpret_cast<ApiEncoder *>(context);
    encoder->addDiagnostic(diag);
}

void ApiEncoder::encodeMsg(llvm::yaml::IO &io, ApiEvent event)
{
    switch (event) {
    case LIBOFP_LISTEN_REQUEST: {
        ApiListenRequest listenReq;
        io.mapRequired("msg", listenReq.msg);
        if (listenReq.msg.listenPort != 0)
            conn_->onListenRequest(&listenReq);
        break;
    }
    case LIBOFP_SET_TIMER: {
        ApiSetTimer setTimer;
        io.mapRequired("msg", setTimer.msg);
        conn_->onSetTimer(&setTimer);
        break;
    }
    default:
        log::info("ApiEncoder: Unrecognized event", event);
        break;
    }
}