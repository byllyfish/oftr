#include "simplecontroller.h"

struct EnetFrame {
	static const EnetFrame *cast(const ByteRange &range)
	{
		return range.size() >= sizeof(EnetFrame) ? reinterpret_cast<EnetFrame *>(range.data()) : nullptr; 
	}

	EnetAddress dest;
	EnetAddress src;
	Big16 type;
};

namespace controller { // <namespace controller>


void SimpleController::onPacketIn(Channel *channel, const PacketIn *msg)
{
    log::debug("SimpleController - PacketIn from:", channel);

    const EnetHeader *frame = EnetFrame::cast(msg->enetFrame());
    if (!frame)
    	return;

    UInt32 inPort = msg->inPort();
    UInt32 outPort;

    // Update forwarding table.
    fwdTable[frame->ethSrc()] = inPort;

   	if (frame->ethDst().isMulticast()) {
   		flood(channel, msg);

   	} else if (!lookupPort(frame->ethDst(), &outPort)) {
   		flood(channel, msg);

   	} else if (outPort == inPort) {
   		drop(channel, msg, 10);

   	} else {
   		addFlow(channel, msg, outPort);
   	}

    log::debug("Packet in: ", RawDataToHex(frame.data(), frame.size()));
}


bool SimpleController::lookupPort(const EnetAddress &addr, UInt32 *port) const
{
	auto iter = fwdTable.find(addr);
	if (iter != fwdTable.end()) {
		*port = iter->second;
		return true;
	}
	return false;
}
	
void SimpleController::flood(Channel *channel, const PacketIn *msg)
{
	ActionList actions;
	actions.add(AT_OUTPUT{OFPP_ALL});

	PacketOutBuilder packetOut{msg};
	packetOut.setActions(actions.actionRange());
	packetOut.send(channel);
}

void SimpleController::drop(Channel *channel, const PacketIn *msg, unsigned timeout)
{
	if (timeout > 0) {
		MatchBuilder match;
		match.setFromPacketIn(msg);

		FlowModBuilder flowMod;
		flowMod.setMatch(match);
		flowMod.setBufferID(msg->bufferID());
		flowMod.setIdleTimeout(timeout);
		flowMod.setHardTimeout(timeout);
		flowMod.send(channel);

	} else {
		PacketOutBuilder packetOut{msg};
		packetOut.send(channel);
	}
}


void SimpleController::addFlow(Channel *channel, const PacketIn *msg, UInt32 outPort)
{
	ActionList actions;
	actions.add(AT_OUTPUT{outPort});

	FlowModBuilder flowMod;
	flowMod.setMatch(match);
	flowMod.setIdleTimeout(timeout);
	flowMod.setHardTimeout(timeout);
	flowMod.setBufferID(msg->bufferID());
	flowMod.send(channel);
}


} // </namespace controller>