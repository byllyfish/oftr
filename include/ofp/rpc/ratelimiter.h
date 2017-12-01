#ifndef OFP_RPC_RATELIMITER_H_
#define OFP_RPC_RATELIMITER_H_

#include "ofp/timestamp.h"

namespace ofp {
namespace rpc {

/// Concrete class that limits the rate of discrete events. This model supports
/// an optional cycle limit every P events:
/// 
/// The basic model is:
/// 
///   N out of P events every T seconds
/// 
/// Examples:
///    N=1 P=Inf T=1     -> Allow 1 event every second
///    N=5 P=Inf T=10    -> Allow 5 events every 10 seconds
///    N=2 P=4   T=10    -> Allow 2 out of 4 events every 10 seconds
///    N=2 P=3   T=Inf   -> Allow 2 out of 3 events forever
///
/// Here is a detailed example that uses N=2, P=4, T=10:
/// 
///    -  0.0s: event 1 is allowed (1 left in 10s)
///    -  2.5s: event 2 is allowed (0 left in 7.5s)
///    -  4.0s: event 3 is DENIED (0 left in 6s)
///    -  5.0s: event 4 is DENIED (0 left in 5s)
///    -  6.5s: event 5 is allowed (cycle restarts; 1 left in 10s)
///    -  9.5s: event 6 is allowed (0 left in 7s)
///    - 10.0s: event 7 is DENIED (0 left in 6.5s)
///
/// Notes:
///   - N >= P is invalid
///   - P == 0 may indicate that P = Inf
///   - T == 0 may indicate that T = Inf
///   - P == 1 is invalid

OFP_BEGIN_IGNORE_PADDING

class RateLimiter {
public:
    explicit RateLimiter(UInt32 n, TimeInterval t) : t_{t}, n_{n}, p_{0xffffffff} {}
    explicit RateLimiter(UInt32 n, UInt32 p, TimeInterval t) : t_{t}, n_{n}, p_{p} {}
    explicit RateLimiter(UInt32 n, UInt32 p) : n_{n}, p_{p} { exp_ = Timestamp::kInfinity; }

    UInt32 n() const { return n_; }
    UInt32 p() const { return p_; }
    TimeInterval t() const { return t_;}

    bool allow(Timestamp t) {
        if (t >= exp_) {
            exp_ = t + t_;
            nc_ = 1;
            return true;
        }

        ++nc_;
        if (nc_ <= n_) {
            return true;
        }

        if (nc_ <= p_) {
            return false;
        }

        if (exp_ != Timestamp::kInfinity) {
            exp_ = t + t_;
        }
        nc_ = 1;
        return true;
    }

private:
    Timestamp exp_;
    const TimeInterval t_;
    UInt32 nc_ = 0;
    const UInt32 n_;
    const UInt32 p_;
};

OFP_END_IGNORE_PADDING

}  // namespace rpc
}  // namespace ofp

#endif // OFP_RPC_RATELIMITER_H_
