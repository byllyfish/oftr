// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YMPREPLYSEQ_H_
#define OFP_YAML_YMPREPLYSEQ_H_

#include "ofp/multipartreply.h"

namespace ofp {
namespace detail {

/// Elements are fixed size.
template <class Type>
class MPReplyFixedSizeSeq {
 public:
  explicit MPReplyFixedSizeSeq(MultipartReply &msg)
      : msg_(msg), position_{msg.replyBody()} {}

  size_t size() const { return msg_.replyBodySize() / sizeof(Type); }

  Type &next() {
    const UInt8 *pos = position_;
    position_ += sizeof(Type);
    return RemoveConst_cast(*reinterpret_cast<const Type *>(pos));
  }

 private:
  MultipartReply &msg_;
  const UInt8 *position_;
};

/// Elements are variable size. MPVariableSizeOffset gives us the offset to the
/// element size.
template <class Type, size_t Offset = Type::MPVariableSizeOffset>
class MPReplyVariableSizeSeq {
 public:
  explicit MPReplyVariableSizeSeq(const UInt8 *buf, size_t size)
      : buf_{buf}, size_{size}, position_{buf} {}

  explicit MPReplyVariableSizeSeq(const MultipartRequest &msg)
      : MPReplyVariableSizeSeq{msg.requestBody(), msg.requestBodySize()} {}
  explicit MPReplyVariableSizeSeq(const MultipartReply &msg)
      : MPReplyVariableSizeSeq{msg.replyBody(), msg.replyBodySize()} {}

  size_t size() const {
    size_t result = 0;
    size_t total = size_;
    const UInt8 *buf = buf_;
    assert(IsPtrAligned(buf, 8));
    size_t len = 0;
    while (len < total) {
      size_t elemSize = PadLength(*Big16_cast(buf + len + Offset));
      assert(elemSize > 0);
      len += elemSize;
      ++result;
    }
    return result;
  }

  Type &next() {
    const UInt8 *pos = position_;
    position_ += PadLength(*Big16_cast(position_ + Offset));
    assert(position_ > pos);
    assert(IsPtrAligned(pos, 8));
    return RemoveConst_cast(*reinterpret_cast<const Type *>(pos));
  }

 private:
  const UInt8 *buf_;
  size_t size_;
  // MultipartReply &msg_;
  const UInt8 *position_;
};

OFP_BEGIN_IGNORE_PADDING

template <class Type>
class MPReplyBuilderSeq {
 public:
  explicit MPReplyBuilderSeq(UInt8 version) : channel_{version}, init_{false} {}

  Type &next() {
    if (init_) {
      item_.write(&channel_);
      item_.reset();
    } else {
      init_ = true;
    }
    return item_;
  }

  void close() { (void)next(); }

  const UInt8 *data() const { return channel_.data(); }
  size_t size() const { return channel_.size(); }

 private:
  MemoryChannel channel_;
  Type item_;
  bool init_;
};

OFP_END_IGNORE_PADDING

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <class Type>
struct SequenceTraits<ofp::detail::MPReplyFixedSizeSeq<Type>> {
  static size_t size(IO &io, ofp::detail::MPReplyFixedSizeSeq<Type> &seq) {
    return seq.size();
  }

  static Type &element(IO &io, ofp::detail::MPReplyFixedSizeSeq<Type> &seq,
                       size_t index) {
    return seq.next();
  }
};

template <class Type>
struct SequenceTraits<ofp::detail::MPReplyVariableSizeSeq<Type>> {
  static size_t size(IO &io, ofp::detail::MPReplyVariableSizeSeq<Type> &seq) {
    return seq.size();
  }

  static Type &element(IO &io, ofp::detail::MPReplyVariableSizeSeq<Type> &seq,
                       size_t index) {
    return seq.next();
  }
};

template <class Type>
struct SequenceTraits<ofp::detail::MPReplyBuilderSeq<Type>> {
  static size_t size(IO &io, ofp::detail::MPReplyBuilderSeq<Type> &seq) {
    return 0;
  }

  static Type &element(IO &io, ofp::detail::MPReplyBuilderSeq<Type> &seq,
                       size_t index) {
    return seq.next();
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YMPREPLYSEQ_H_
