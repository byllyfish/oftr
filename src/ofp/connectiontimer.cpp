#include "ofp/sys/connectiontimer.h"
#include "ofp/sys/connection.h"
#include "ofp/sys/engine.h"


namespace ofp { // <namespace ofp>
namespace sys { // <namespace sys>


ConnectionTimer::ConnectionTimer(Connection *conn, UInt32 id, milliseconds interval, bool repeating)
	: conn_{conn}, timer_{conn->engine()->io()}, interval_{interval}, id_{id}, repeating_{repeating}
{
	asyncWait();
}


void ConnectionTimer::asyncWait()
{
	log::debug("ConnectionTimer::asyncWait");

	timer_.expires_from_now(interval_);
	timer_.async_wait([this](const error_code &err) {
		if (err != boost::asio::error::operation_aborted) {
			// If we're repeating, restart the timer before posting it. It's
			// possible the client will want to cancel the timer when they
			// receive its event.
			if (repeating_) {
				asyncWait();	
			}
			conn_->postTimerExpired(this);
		}
	});
}


} // </namespace sys>
} // </namespace ofp>
