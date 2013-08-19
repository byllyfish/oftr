#ifndef OFP_SIMPLECONTROLLER_H
#define OFP_SIMPLECONTROLLER_H

#include "ofp.h"

namespace controller { // <namespace controller>

using namespace ofp;


class SimpleController {
public:

	void onPacketIn(Channel *source, const PacketIn *msg);
	//void onFlowRemoved(Channnel *source, FlowRemoved *message);
	//void onPortStatus(Channel *source, PortStatus *message);

};

} // </namespace controller>

#endif // OFP_SIMPLECONTROLLER_H
