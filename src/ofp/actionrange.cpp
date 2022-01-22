// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/actionrange.h"

#include "ofp/actionlist.h"
#include "ofp/actions.h"
#include "ofp/nicira.h"
#include "ofp/oxmfields.h"

using namespace ofp;

static bool validateInput_SetField(const detail::ActionIteratorItem &item,
                                   Validation *context) {
  size_t len = item.size();

  // Check SetField action size.
  if (len < 16) {
    return false;
  }

  // Check oxm field.
  OXMRange oxm = item.oxmRange();
  if (!oxm.validateInput_paddedField()) {
    log_error("SetField action has invalid oxm field");
    return false;
  }

  return true;
}

// Delegating constructor.
ActionRange::ActionRange(const ActionList &list)
    : ActionRange{list.toRange()} {}

bool ActionRange::validateInput(Validation *context) const {
  if (!Inherited::validateInput(context))
    return false;

  for (auto &item : *this) {
    switch (item.type().enumType()) {
      case OFPAT_SET_FIELD: {
        if (!validateInput_SetField(item, context))
          return false;
        break;
      }
      case OFPAT_EXPERIMENTER: {
        auto action = item.action<AT_EXPERIMENTER>();
        if (!action->validateInput(context))
          return false;
        if (action->experimenterid() == nx::NICIRA) {
          if (!validateInput_NICIRA(action, context))
            return false;
        }
        break;
      }
      default:
        break;
    }
  }

  return true;
}

/// \returns Size of action list when written to channel.
size_t ActionRange::writeSize(Writable *channel) {
  UInt8 version = channel->version();

  if (version != OFP_VERSION_1) {
    return size();
  }

  size_t newSize = size();

  ActionIterator iter = begin();
  ActionIterator iterEnd = end();

  while (iter != iterEnd) {
    ActionType type = iter->type();

    if (type == AT_OUTPUT::type()) {
      newSize -= 8;
    } else if (type.enumType() == OFPAT_SET_FIELD) {
      newSize -= writeSizeMinusSetFieldV1(iter);
    }

    ++iter;
  }

  return newSize;
}

/// \brief Writes action list to channel.
void ActionRange::write(Writable *channel) {
  UInt8 version = channel->version();

  // Only fix up actions for version 1 (TODO).

  if (version != OFP_VERSION_1) {
    channel->write(data(), size());
    return;
  }

  // Translate AT_OUTPUT's port number from 32 to 16 bits on write.

  ActionIterator iter = begin();
  ActionIterator iterEnd = end();

  while (iter != iterEnd) {
    // Write each action separately.
    ActionType type = iter->type();

    if (type == AT_OUTPUT::type()) {
      deprecated::AT_OUTPUT_V1 outAction{iter->action<AT_OUTPUT>()};
      channel->write(&outAction, sizeof(outAction));
    } else if (type.enumType() == OFPAT_SET_FIELD) {
      writeSetFieldV1(iter, channel);
    } else {
      channel->write(iter.data(), type.length());
    }

    ++iter;
  }
}

namespace {

template <class Action, class OFBType>
void writeAction(ActionIterator iter, Writable *channel) {
  Action action{iter->action<AT_SET_FIELD<OFBType>>()};
  channel->write(&action, sizeof(action));
}

}  // namespace

unsigned ActionRange::writeSizeMinusSetFieldV1(ActionIterator iter) {
  using namespace deprecated;

  OXMIterator oxm = iter->oxmRange().begin();
  switch (oxm->type()) {
    case OFB_ETH_SRC::type():
    case OFB_ETH_DST::type():
      return 0;
    case OFB_VLAN_VID::type():
    case OFB_VLAN_PCP::type():
    case OFB_IPV4_SRC::type():
    case OFB_IPV4_DST::type():
    case OFB_IP_DSCP::type():
    case OFB_TCP_SRC::type():
    case OFB_TCP_DST::type():
    case OFB_UDP_SRC::type():
    case OFB_UDP_DST::type():
    case OFB_ICMPV4_TYPE::type():
    case OFB_ICMPV4_CODE::type():
      return 8;
    default:
      log_debug("ActionRange::writeSizeMinusSetFieldV1: Unexpected OXM type:",
                oxm->type());
      return 0;
  }
}

void ActionRange::writeSetFieldV1(ActionIterator iter, Writable *channel) {
  using namespace deprecated;

  OXMIterator oxm = iter->oxmRange().begin();
  switch (oxm->type()) {
    case OFB_VLAN_VID::type():
      writeAction<AT_SET_VLAN_VID_V1, OFB_VLAN_VID>(iter, channel);
      break;
    case OFB_VLAN_PCP::type():
      writeAction<AT_SET_VLAN_PCP_V1, OFB_VLAN_PCP>(iter, channel);
      break;
    case OFB_ETH_SRC::type():
      writeAction<AT_SET_DL_SRC_V1, OFB_ETH_SRC>(iter, channel);
      break;
    case OFB_ETH_DST::type():
      writeAction<AT_SET_DL_DST_V1, OFB_ETH_DST>(iter, channel);
      break;
    case OFB_IPV4_SRC::type():
      writeAction<AT_SET_NW_SRC_V1, OFB_IPV4_SRC>(iter, channel);
      break;
    case OFB_IPV4_DST::type():
      writeAction<AT_SET_NW_DST_V1, OFB_IPV4_DST>(iter, channel);
      break;
    case OFB_IP_DSCP::type():
      writeAction<AT_SET_NW_TOS_V1, OFB_IP_DSCP>(iter, channel);
      break;
    case OFB_TCP_SRC::type():
      writeAction<AT_SET_TP_SRC_V1, OFB_TP_PORT>(iter, channel);
      break;
    case OFB_TCP_DST::type():
      writeAction<AT_SET_TP_DST_V1, OFB_TP_PORT>(iter, channel);
      break;
    case OFB_UDP_SRC::type():
      writeAction<AT_SET_TP_SRC_V1, OFB_TP_PORT>(iter, channel);
      break;
    case OFB_UDP_DST::type():
      writeAction<AT_SET_TP_DST_V1, OFB_TP_PORT>(iter, channel);
      break;
    case OFB_ICMPV4_TYPE::type():
      writeAction<AT_SET_TP_TYPE_V1, OFB_TP_CODE>(iter, channel);
      break;
    case OFB_ICMPV4_CODE::type():
      writeAction<AT_SET_TP_CODE_V1, OFB_TP_CODE>(iter, channel);
      break;
    default:
      log_debug("ActionRange::writeSetFieldV1: Unknown field type: ",
                oxm->type());
      break;
  }
}

bool ActionRange::validateInput_NICIRA(const AT_EXPERIMENTER *action,
                                       Validation *context) {
  assert(action->experimenterid() == nx::NICIRA);

  size_t size = action->size();
  if (size < AT_EXPERIMENTER::MinSizeWithSubtype) {
    return context->validateBool(false, "Invalid Nicira action size");
  }

  switch (action->subtype()) {
    case nx::NXAST_REG_MOVE:
      return nx::AT_REGMOVE::cast(action)->validateInput(context);
    case nx::NXAST_REG_LOAD:
      return nx::AT_REGLOAD::cast(action)->validateInput(context);
    default:
      break;
  }

  return true;
}
