// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_MOCKCHANNEL_H_
#define OFP_MOCKCHANNEL_H_

#include "ofp/bytelist.h"
#include "ofp/channel.h"
#include "ofp/constants.h"
#include "ofp/padding.h"

namespace ofp {

// \brief Concrete class to use for a Channel for testing purposes.
//
// There is a related class called `MemoryChannel` which is a subclass
// of `Writable`; MemoryChannel should be preferred where possible.

class MockChannel : public Channel {
 public:
  explicit MockChannel(UInt8 version = OFP_VERSION_LAST) : version_{version} {}

  const UInt8 *data() const {
    assert(flushed_ || size() == 0);
    return buf_.data();
  }

  size_t size() const { return buf_.size(); }

  const UInt8 *begin() const { return buf_.begin(); }
  const UInt8 *end() const { return buf_.end(); }

  UInt8 version() const override { return version_; }
  void setVersion(UInt8 version) { version_ = version; }

  void write(const void *data, size_t length) override {
    buf_.add(data, length);
  }

  void flush() override { flushed_ = true; }
  bool mustFlush() const override { return false; }

  UInt32 nextXid() override { return nextXid_++; }
  void setNextXid(UInt32 xid) { nextXid_ = xid; }

  void clear() { buf_.clear(); }

  Driver *driver() const override { return nullptr; }
  UInt64 connectionId() const override { return 1; }
  DatapathID datapathId() const override { return {}; }
  UInt8 auxiliaryId() const override { return 0; }
  ChannelTransport transport() const override {
    return ChannelTransport::TCP_Plaintext;
  }
  IPv6Endpoint remoteEndpoint() const override { return {}; }
  IPv6Endpoint localEndpoint() const override { return {}; }
  void shutdown(bool reset = false) override {}
  void tickle(TimePoint now) override {}

  Milliseconds keepAliveTimeout() const override { return {}; }
  void setKeepAliveTimeout(const Milliseconds &timeout) override {}

  ChannelListener *channelListener() const override { return nullptr; }
  void setChannelListener(ChannelListener *listener) override {}
  void setStartingXid(UInt32 xid) override {}

 private:
  ByteList buf_;
  UInt32 nextXid_ = 1;
  UInt8 version_;
  bool flushed_ = false;
  Padding<2> pad_;
};

}  // namespace ofp

#endif  // OFP_MOCKCHANNEL_H_
