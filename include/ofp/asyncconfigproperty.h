// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_ASYNCCONFIGPROPERTY_H_
#define OFP_ASYNCCONFIGPROPERTY_H_

#include "ofp/experimenterproperty.h"

namespace ofp {
namespace detail {

template <class ValueT, OFPAsyncConfigProperty Type>
class AsyncConfigProperty : private NonCopyable {
 public:
  using ValueType = ValueT;

  constexpr static OFPAsyncConfigProperty type() { return Type; }

  explicit AsyncConfigProperty(ValueType value) : value_{value} {}

  ValueType value() const { return value_; }
  static ValueType defaultValue() { return {}; }

 private:
  Big16 type_ = type();
  Big16 length_ = 8;
  Big<ValueType> value_;

  static_assert(sizeof(ValueType) == 4, "Unexpected size.");
};

}  // namespace detail

using AsyncConfigPropertyPacketInSlave =
    detail::AsyncConfigProperty<OFPPacketInFlags, OFPACPT_PACKET_IN_SLAVE>;
using AsyncConfigPropertyPacketInMaster =
    detail::AsyncConfigProperty<OFPPacketInFlags, OFPACPT_PACKET_IN_MASTER>;
using AsyncConfigPropertyPortStatusSlave =
    detail::AsyncConfigProperty<OFPPortStatusFlags, OFPACPT_PORT_STATUS_SLAVE>;
using AsyncConfigPropertyPortStatusMaster =
    detail::AsyncConfigProperty<OFPPortStatusFlags, OFPACPT_PORT_STATUS_MASTER>;
using AsyncConfigPropertyFlowRemovedSlave =
    detail::AsyncConfigProperty<OFPFlowRemovedFlags,
                                OFPACPT_FLOW_REMOVED_SLAVE>;
using AsyncConfigPropertyFlowRemovedMaster =
    detail::AsyncConfigProperty<OFPFlowRemovedFlags,
                                OFPACPT_FLOW_REMOVED_MASTER>;
using AsyncConfigPropertyRoleStatusSlave =
    detail::AsyncConfigProperty<OFPRoleStatusFlags, OFPACPT_ROLE_STATUS_SLAVE>;
using AsyncConfigPropertyRoleStatusMaster =
    detail::AsyncConfigProperty<OFPRoleStatusFlags, OFPACPT_ROLE_STATUS_MASTER>;
using AsyncConfigPropertyTableStatusSlave =
    detail::AsyncConfigProperty<OFPTableStatusFlags,
                                OFPACPT_TABLE_STATUS_SLAVE>;
using AsyncConfigPropertyTableStatusMaster =
    detail::AsyncConfigProperty<OFPTableStatusFlags,
                                OFPACPT_TABLE_STATUS_MASTER>;
using AsyncConfigPropertyRequestForwardSlave =
    detail::AsyncConfigProperty<OFPRequestForwardFlags,
                                OFPACPT_REQUESTFORWARD_SLAVE>;
using AsyncConfigPropertyRequestForwardMaster =
    detail::AsyncConfigProperty<OFPRequestForwardFlags,
                                OFPACPT_REQUESTFORWARD_MASTER>;

using AsyncConfigPropertyExperimenterSlave =
    detail::ExperimenterProperty<OFPAsyncConfigProperty,
                                 OFPACPT_EXPERIMENTER_SLAVE>;
using AsyncConfigPropertyExperimenterMaster =
    detail::ExperimenterProperty<OFPAsyncConfigProperty,
                                 OFPACPT_EXPERIMENTER_MASTER>;

}  // namespace ofp

#endif  // OFP_ASYNCCONFIGPROPERTY_H_
