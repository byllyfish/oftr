// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/log.h"

#include "ofp/timestamp.h"
#include "ofp/yaml/decoder.h"

namespace ofp {
namespace log {
namespace detail {

static void trace1(const char *type, UInt64 id, const void *data,
                   size_t length) {
  if (length < sizeof(Header)) {
    detail::write_(Level::Trace, type, length,
                   "Invalid Data:", RawDataToHex(data, length));
    return;
  }

  Message message{data, length};

  // Do not trace echo requests or echo replies.
  if (message.type() == OFPT_ECHO_REQUEST || message.type() == OFPT_ECHO_REPLY)
    return;

  message.normalize();

  yaml::Decoder decoder{&message};

  if (decoder.error().empty()) {
    detail::write_(Level::Trace, type, length, "bytes",
                   std::make_pair("connid", id), '\n', decoder.result(),
                   RawDataToHex(data, length));
  } else {
    detail::write_(Level::Trace, type, length, "bytes",
                   std::make_pair("connid", id), '\n', decoder.error(),
                   RawDataToHex(data, length));
  }
}

void trace_msg_(const char *type, UInt64 id, const void *data, size_t length) {
  // The memory buffer may contain multiple messages. We need to log each one
  // separately.

  size_t remaining = length;
  const UInt8 *ptr = BytePtr(data);

  const Header *header = reinterpret_cast<const Header *>(ptr);
  while (remaining >= sizeof(Header) && header->length() <= remaining) {
    trace1(type, id, ptr, header->length());

    remaining -= header->length();
    ptr += header->length();
    header = reinterpret_cast<const Header *>(ptr);
  }

  if (remaining > 0) {
    detail::write_(Level::Trace, type,
                   "Invalid Leftover:", RawDataToHex(ptr, remaining));
  }
}

void trace_rpc_(const char *type, UInt64 id, const void *data, size_t length) {
  const char *msg = static_cast<const char *>(data);

  // Remove trailing zero, if it exists.
  if (length > 0 && msg[length - 1] == '\0')
    --length;

  // If the message contains a private key, we need to obscure the private
  // information.
  llvm::StringRef str{msg, length};
  if (str.contains("OFP.ADD_IDENTITY")) {
    str = "{*** OFP.ADD_IDENTITY ELIDED ***}";
  }

  detail::write_(Level::Trace, type, length, "bytes",
                 std::make_pair("connid", id), '\n', str);
}

}  // namespace detail
}  // namespace log
}  // namespace ofp
