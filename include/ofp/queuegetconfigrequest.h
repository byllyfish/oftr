// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_QUEUEGETCONFIGREQUEST_H_
#define OFP_QUEUEGETCONFIGREQUEST_H_

#include "ofp/padding.h"
#include "ofp/portnumber.h"
#include "ofp/protocolmsg.h"

namespace ofp {

// Note: This message is replaced by a MultipartRequest in version 1.4.

class QueueGetConfigRequest
    : public ProtocolMsg<QueueGetConfigRequest, OFPT_QUEUE_GET_CONFIG_REQUEST,
                         16, 16> {
 public:
  PortNumber port() const { return port_; }

  bool validateInput(Validation *context) const { return true; }

 private:
  Header header_;
  PortNumber port_;
  Padding<4> pad_;

  QueueGetConfigRequest() : header_{type()} {}

  friend class QueueGetConfigRequestBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(QueueGetConfigRequest) == 16, "Unexpected size.");
static_assert(IsStandardLayout<QueueGetConfigRequest>(),
              "Expected standard layout.");
static_assert(IsTriviallyCopyable<QueueGetConfigRequest>(),
              "Expected trivially copyable.");

class QueueGetConfigRequestBuilder {
 public:
  QueueGetConfigRequestBuilder() = default;
  explicit QueueGetConfigRequestBuilder(const QueueGetConfigRequest *msg);

  void setPort(PortNumber port) { msg_.port_ = port; }

  UInt32 send(Writable *channel);

 private:
  QueueGetConfigRequest msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_QUEUEGETCONFIGREQUEST_H_
