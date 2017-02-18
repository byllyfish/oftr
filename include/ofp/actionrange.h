// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_ACTIONRANGE_H_
#define OFP_ACTIONRANGE_H_

#include "ofp/actioniterator.h"
#include "ofp/actions.h"
#include "ofp/byterange.h"
#include "ofp/protocolrange.h"
#include "ofp/writable.h"

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

  static bool validateInput_NICIRA(const AT_EXPERIMENTER *action,
                                   Validation *context);
};

}  // namespace ofp

#endif  // OFP_ACTIONRANGE_H_
