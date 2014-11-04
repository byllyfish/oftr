// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_API_APICHANNELLISTENER_H_
#define OFP_API_APICHANNELLISTENER_H_

#include "ofp/channellistener.h"

namespace ofp {
namespace api {

class ApiServer;

class ApiChannelListener : public ChannelListener {
 public:
  explicit ApiChannelListener(ApiServer *server) : server_{server} {}

  void onChannelUp(Channel *channel) override;
  void onChannelDown(Channel *channel) override;
  void onMessage(const Message *message) override;

 private:
  ApiServer *server_;
  Channel *channel_ = nullptr;
};

}  // namespace api
}  // namespace ofp

#endif  // OFP_API_APICHANNELLISTENER_H_
