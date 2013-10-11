#ifndef OFP_PROTOCOLMSG_H
#define OFP_PROTOCOLMSG_H

#include "ofp/header.h"
#include "ofp/message.h"

namespace ofp { // <namespace ofp>

/// \brief Static base class template for all protocol message types. This class
/// implements the two static methods, type() and cast(), common to all protocol
/// message types.

template <class MsgClass, OFPType MsgType>
class ProtocolMsg {
public:

	/// \returns OpenFlow message type reprented by this class.
    static constexpr OFPType type()
    {
        return MsgType;
    }

    /// \returns Pointer to message memory as given class or nullptr.
    static const MsgClass *cast(const Message *message)
    {
        return message->cast<MsgClass>();
    }
};

} // </namespace ofp>

#endif // OFP_PROTOCOLMSG_H
