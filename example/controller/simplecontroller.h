#ifndef OFP_SIMPLECONTROLLER_H
#define OFP_SIMPLECONTROLLER_H

#include "ofp.h"
#include <unordered_map>

namespace controller { // <namespace controller>

using namespace ofp;

struct EnetFrame;


class SimpleController {
public:

	void onPacketIn(Channel *channel, const PacketIn *msg);
	void onPortStatus(Channel *channel, const PortStatus *msg);
	void onError(Channel *channel, const Error *msg);
	
private:
	using FwdTable = std::unordered_map<EnetAddress,UInt32>;

	FwdTable fwdTable_;

	bool lookupPort(const EnetAddress &addr, UInt32 *port) const;

	static void flood(Channel *channel, const PacketIn *msg);
	static void drop(Channel *channel, const PacketIn *msg, const EnetFrame *frame, unsigned timeout);
	static void addFlow(Channel *channel, const PacketIn *msg, const EnetFrame *frame, UInt32 outPort);
};

} // </namespace controller>

#endif // OFP_SIMPLECONTROLLER_H
