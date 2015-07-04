// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_ACTIONRANGE_H_
#define OFP_ACTIONRANGE_H_

#include "ofp/protocolrange.h"
#include "ofp/byterange.h"
#include "ofp/writable.h"
#include "ofp/actioniterator.h"

namespace ofp {

class ActionList;

class ActionRange : public ProtocolRange<ActionIterator> {
  using Inherited = ProtocolRange<ActionIterator>;

 public:
  using Inherited::Inherited;

  ActionRange() = default;
  /* implicit NOLINT */ ActionRange(const ActionList &list);

  bool validateInput(Validation *context) const;
  
  /// \returns Size of action list when written to channel using the specified
  /// protocol version.
  size_t writeSize(Writable *channel);

  /// \brief Writes action list to the channel using the specified protocol
  /// version.
  void write(Writable *channel);

 private:
  static unsigned writeSizeMinusSetFieldV1(ActionIterator iter);
  static void writeSetFieldV1(ActionIterator iter, Writable *channel);
};

}  // namespace ofp

#endif  // OFP_ACTIONRANGE_H_
