// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "./libofp.h"
#include <stdlib.h>
#include <string>
#include "ofp/log.h"
#include "ofp/yaml/encoder.h"
#include "ofp/yaml/decoder.h"

static void buf_set(libofp_buffer *buf, const void *data, size_t len) {
  char *newbuf = static_cast<char *>(malloc(len + 1));
  ofp::log::fatal_if_null(newbuf);
  memcpy(newbuf, data, len);
  newbuf[len] = 0;
  buf->data = newbuf;
  buf->length = len;
}

static void buf_set(libofp_buffer *buf, const llvm::StringRef &val) {
  buf_set(buf, val.data(), val.size());
}

void libofp_version(libofp_buffer *result) {
  std::string libofpCommit{LIBOFP_GIT_COMMIT_LIBOFP};
  std::ostringstream oss;

  oss << LIBOFP_VERSION_STRING << " (" << libofpCommit.substr(0, 7) << ")";

  oss << "  <" << LIBOFP_GITHUB_URL << ">";

  buf_set(result, oss.str());
}

int libofp_encode(libofp_buffer *result, const char *yaml_input, uint32_t flags) {
  // `flags` unused for now
  llvm::StringRef text{yaml_input};
  ofp::yaml::Encoder encoder{text, false, 1, 0};

  auto err = encoder.error();
  if (!err.empty()) {
    buf_set(result, err);
    return -1;
  }

  buf_set(result, encoder.data(), encoder.size());
  return 0;
}

int libofp_decode(libofp_buffer *result, const libofp_buffer *input,
                  uint32_t flags) {
  // `flags` unused for now

  if (input->length < sizeof(ofp::Header)) {
    buf_set(result, "Buffer size < 8 bytes");
    return -1;
  }

  if (input->length > 0xffff) {
    buf_set(result, "Buffer size > 65535 bytes");
    return -1;
  }

  if (ofp::Big16_copy(ofp::BytePtr(input->data) + 2) != input->length) {
    buf_set(result, "Message length does not match buffer size");
    return -1;
  }

  ofp::Message message{input->data, input->length};
  message.transmogrify();

  ofp::yaml::Decoder decoder{&message, false, false};

  auto err = decoder.error();
  if (!err.empty()) {
    buf_set(result, err);
    return -1;
  }

  buf_set(result, decoder.result());
  return 0;
}

void libofp_buffer_free(libofp_buffer *buffer) {
  if (buffer->data) {
    free(buffer->data);
    buffer->data = nullptr;
    buffer->length = 0;
  }
}
