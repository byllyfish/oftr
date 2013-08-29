#ifndef OFP_SYS_CONNECTION_TIMER_H
#define OFP_SYS_CONNECTION_TIMER_H

#include "ofp/sys/boost_asio.h"

namespace ofp { // <namespace ofp>
namespace sys { // <namespace sys>

class Connection;

OFP_BEGIN_IGNORE_PADDING

/// \brief Implements a timer acting on a specified connection. Each timer has a
/// unique ID.
class ConnectionTimer {
public:
	ConnectionTimer(Connection *conn, UInt32 id, milliseconds interval, bool repeating);

	UInt32 id() const { return id_; }
	bool repeating() const { return repeating_; }

private:
	Connection *conn_;
	steady_timer timer_;
	milliseconds interval_;
    UInt32 id_;
    bool repeating_;

    void asyncWait();
};

OFP_END_IGNORE_PADDING

/// \brief Stores connection timers so we can look them up by ID.
using ConnectionTimerMap = std::map<UInt32,std::unique_ptr<ConnectionTimer>>;

} // </namespace sys>
} // </namespace ofp>

#endif // OFP_SYS_CONNECTION_TIMER_H
