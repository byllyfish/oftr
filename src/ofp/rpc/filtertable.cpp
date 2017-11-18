#include "ofp/rpc/filtertable.h"
#include "ofp/message.h"
#include "ofp/packetin.h"

using namespace ofp::rpc;

// Apply an OF message against the filter table.
//
// We apply the message against each table entry in order until one matches.
//
// For a filter entry F, F(M, escalate) returns true when the filter criteria
// and
// action matches the message. A filter entry that matches may modify the
// 'escalate' flag to block the message from proceeding up the chain of command.
// The filter action may modify the message to add flags (e.g. REPLIED,
// METERED, etc.)

bool FilterTable::apply(Message *message, bool *escalate) {
  // The filter table only works on PacketIn messages.
  if (message->type() != OFPT_PACKET_IN) {
    return false;
  }

  const PacketIn *packetIn = PacketIn::cast(message);
  if (!packetIn) {
    log_warning("FilterTable::apply - unable to decode packetIn");
    return false;
  }

  ByteRange data = packetIn->enetFrame();
  PortNumber inPort = packetIn->inPort();

  for (auto &entry : table_) {
    if (entry.apply(data, inPort, message, escalate)) {
      return true;
    }
  }

  return false;
}
