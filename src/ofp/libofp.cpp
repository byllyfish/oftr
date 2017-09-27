// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/libofp.h"
#include <stdlib.h>
#include <string>
#include "ofp/log.h"
#include "ofp/yaml/decoder.h"
#include "ofp/yaml/encoder.h"

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
  std::string buf;
  llvm::raw_string_ostream oss{buf};

  oss << LIBOFP_VERSION_STRING << " (" << libofpCommit.substr(0, 7) << ")";
  oss << "  <" << LIBOFP_GITHUB_URL << ">";

  buf_set(result, oss.str());
}

int libofp_encode(libofp_buffer *result, const char *yaml_input,
                  uint32_t flags) {
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

  if (ofp::Big16_unaligned(ofp::BytePtr(input->data) + 2) != input->length) {
    buf_set(result, "Message length does not match buffer size");
    return -1;
  }

  ofp::Message message{input->data, input->length};
  message.normalize();

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

const uint32_t kMaxSupportedLen = 1073741823;
const int32_t kInvalidArgumentsError = -1;
const int32_t kInternalError = -2;

static int32_t buf_copy(char *output, size_t output_len, const void *data,
                        size_t len, bool error) {
  if (len > kMaxSupportedLen) {
    return kInternalError;
  }

  int32_t result = static_cast<int32_t>(len);
  if (len > output_len || !output) {
    return -result;
  }

  assert(len <= output_len);
  std::memcpy(output, data, len);

  if (error) {
    return -result;
  }

  return result;
}

static int32_t buf_copy(char *output, size_t output_len, const char *str,
                        bool error) {
  return buf_copy(output, output_len, str, strlen(str), error);
}

static int32_t oftr_version(char *output, size_t output_len) {
  std::string libofpCommit{LIBOFP_GIT_COMMIT_LIBOFP};
  std::string buf;
  llvm::raw_string_ostream oss{buf};

  oss << LIBOFP_VERSION_STRING << " (" << libofpCommit.substr(0, 7) << ")";
  oss << "  <" << LIBOFP_GITHUB_URL << ">";
  oss.flush();

  return buf_copy(output, output_len, buf.data(), buf.size(), false);
}

static int32_t oftr_encode(const char *input, size_t input_len, uint8_t version,
                           char *output, size_t output_len) {
  llvm::StringRef text{input, input_len};
  ofp::yaml::Encoder encoder{text, true, 1, version};

  auto err = encoder.error();
  if (!err.empty()) {
    return buf_copy(output, output_len, err.data(), err.size(), true);
  }

  return buf_copy(output, output_len, encoder.data(), encoder.size(), false);
}

static int32_t oftr_decode(const char *input, size_t input_len, char *output,
                           size_t output_len) {
  if (input_len < sizeof(ofp::Header)) {
    return buf_copy(output, output_len, "Buffer size < 8 bytes", true);
  }

  if (input_len > 0xffff) {
    return buf_copy(output, output_len, "Buffer size > 65535 bytes", true);
  }

  if (ofp::Big16_unaligned(ofp::BytePtr(input) + 2) != input_len) {
    return buf_copy(output, output_len,
                    "Message length does not match buffer size", true);
  }

  ofp::Message message{input, input_len};
  message.normalize();

  ofp::yaml::Decoder decoder{&message, false, false};

  auto err = decoder.error();
  if (!err.empty()) {
    return buf_copy(output, output_len, err.data(), err.size(), true);
  }

  auto result = decoder.result();
  return buf_copy(output, output_len, result.data(), result.size(), false);
}

int32_t oftr_call(uint32_t opcode, const char *input, size_t input_len,
                  char *output, size_t output_len) {
  if ((input_len > kMaxSupportedLen) || (output_len > kMaxSupportedLen) ||
      (!input && input_len > 0) || (!output && output_len > 0)) {
    return kInvalidArgumentsError;
  }

  switch (opcode & 0x00FF) {
    case OFTR_VERSION:
      return oftr_version(output, output_len);
    case OFTR_ENCODE:
      return oftr_encode(input, input_len, (opcode >> 24), output, output_len);
    case OFTR_DECODE:
      return oftr_decode(input, input_len, output, output_len);
    default:
      return kInvalidArgumentsError;
  }
}
