#ifndef OFP_PROTOCOLITERABLE_H
#define OFP_PROTOCOLITERABLE_H

#include "ofp/protocoliterator.h"
#include "ofp/byterange.h"
#include "ofp/log.h"

namespace ofp { // <namespace ofp>

namespace detail { // <namespace detail>

/// Return true if given ByteRange is a valid protocol iterable.
///
/// To be structurally valid as a ProtocolIterable, the byte range must pass
/// the following tests:
///   - size in bytes is a multiple of 8.
///   - no element length is less than 4.
///   - the sum of each element length (padded out to 8-bytes) evenly matches
///     the size.
///   - pointer to start of range is 8-byte aligned.
///
/// \return true if byte range is a valid protocol iterable.
bool IsProtocolIterableValid(const ByteRange &range, const char *context="");

/// Return count of items in the protocol iterable.
///
/// If protocol iterable is not valid, return count of valid items.
///
/// \return number of items in iterable.
size_t ProtocolIterableItemCount(const ByteRange &range);

} // </namespace detail>

template <class ElemType>
class ProtocolIterable {
public:
  using Iterator = ProtocolIterator<ElemType>;

  // constructor is implicitly convertable from ByteRange.
  ProtocolIterable(const ByteRange &range) : range_{range} {}

  Iterator begin() const { return Iterator(range_.begin()); }
  Iterator end() const { return Iterator(range_.end()); }

  size_t itemCount() const { return detail::ProtocolIterableItemCount(range_); }

  bool validateInput(const char *context) const {
    return detail::IsProtocolIterableValid(range_, context);
  }

private:
  const ByteRange range_;
};

} // </namespace ofp>

#endif // OFP_PROTOCOLITERABLE_H
