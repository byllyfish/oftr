//  ===== ---- ofp/yaml/encoder.cpp ------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Implements yaml::Encoder class.
//  ===== ------------------------------------------------------------ =====  //

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

namespace ofp {  // <namespace ofp>
namespace yaml { // <namespace yaml>


Encoder::Encoder(const std::string &input, ChannelFinder finder) : errorStream_{error_}, finder_{finder}
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
    // At this point, we know the datapathID. The YAML message may not contain
    // the version and xid, and even if it does, we still need to override the
    // version if the channel corresponds to an actual channel. Use the channel
    // finder to locate the channel for the given datapathID so we can set the
    // correct protocol version and possibly override the xid.
    
    Channel *actualChannel = finder_(datapathId_);
    if (actualChannel) {
        channel_.setVersion(actualChannel->version());
        if (header.xid() == 0) {
            channel_.setNextXid(actualChannel->nextXid());
        }
    } else {
        channel_.setVersion(header.version());
        channel_.setNextXid(header.xid());
    }

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
    default:
        log::info("yaml::Encoder::encodeMsg: Unsupported message type:", int(header.type()));
        break;
    }
}

Channel *Encoder::NullChannelFinder(const DatapathID &datapathId)
{
    return nullptr;
}

} // </namespace yaml>
} // </namespace ofp>