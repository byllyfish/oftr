// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_CHANNELLISTENER_H_
#define OFP_CHANNELLISTENER_H_

#include <functional>
#include "ofp/types.h"

namespace ofp {

class Channel;
class Message;
class Exception;

namespace sys {
class Connection;
}  // namespace sys

/// \brief An abstract class for listening to events and messages from a
/// Channel.
class ChannelListener {
 public:
  using Factory = std::function<ChannelListener *()>;

  // Lifecycle:
  //
  // Call onChannelUp when connection is up and channel is ready. Called one
  // time. (See exception below.)
  // Call onChannelDown when connection goes down. Called one time. (See
  // exception below.)
  // Call onMessage zero or more times.

  virtual void onChannelUp(Channel *channel);
  virtual void onChannelDown(Channel *channel);
  virtual void onMessage(Message *message) = 0;

  // When a controller identifies an incoming connection as an auxiliary
  // connection, the driver will transparently tie the auxiliary connection
  // to the main connection. onChannelUp() and onChannelDown() will _NOT_ be
  // called for auxiliary connections that the engine did not initiate.
  // The listener may receive messages from the auxiliary connection; replies
  // can just be sent to message->source().
  //
  // If an agent actively opens an auxiliary channel (via connect),
  // onChannelUp() and onChannelDown() _WILL_ be called multiple times.

  /// Called to periodically tickle the connection, so your handler can detect
  /// timeouts. If this function returns false, the connection's default "poll"
  /// function also runs.
  virtual bool onTickle(Channel *channel, TimePoint now) { return false; }

 protected:
  // ChannelListeners must be allocated on the heap; never on the stack.
  // After detaching from a Channel, a ChannelListener may delete itself
  // using ChannelListener::dispose(this).

  virtual ~ChannelListener() {}

  static void dispose(ChannelListener *listener);

  friend class sys::Connection;
};

}  // namespace ofp

#endif  // OFP_CHANNELLISTENER_H_
