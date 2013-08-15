#ifndef OFP_TIMER_H
#define OFP_TIMER_H

#include "ofp/types.h"

namespace ofp { // <namespace ofp>

class InternalChannel;
class InternalTimer;


class Timer {
public:
	Timer(InternalChannel *channel);

	void reschedule();
	void cancel();

private:
	std::unique_ptr<InternalTimer> impl_;
};

} // </namespace ofp>

#endif // OFP_TIMER_H
