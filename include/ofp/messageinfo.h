// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_MESSAGEINFO_H_
#define OFP_MESSAGEINFO_H_

#include "ofp/ipv6endpoint.h"

namespace ofp {

OFP_BEGIN_IGNORE_PADDING

class MessageInfo {
 public:
  MessageInfo() : available_{false} {}
  explicit MessageInfo(UInt64 sessionId, const IPv6Endpoint &src,
                       const IPv6Endpoint &dst)
      : sessionId_{sessionId}, source_{src}, dest_{dst} {}
  explicit MessageInfo(const std::string filename) : filename_{filename} {}

  bool available() const { return available_; }
  UInt64 sessionId() const { return sessionId_; }
  IPv6Endpoint source() const { return source_; }
  IPv6Endpoint dest() const { return dest_; }

  // Information about the message source if it's a file.
  const std::string &filename() const { return filename_; }

  const std::string &errorMessage() const { return error_; }
  void setErrorMessage(const std::string &error) { error_ = error; }

 private:
  UInt64 sessionId_ = 0;
  IPv6Endpoint source_;
  IPv6Endpoint dest_;
  bool available_ = true;
  std::string filename_;
  std::string error_;
};

OFP_END_IGNORE_PADDING

}  // namespace ofp

#endif  // OFP_MESSAGEINFO_H_
