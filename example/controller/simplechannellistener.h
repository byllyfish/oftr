#include "ofp/ofp.h"
#include "simplecontroller.h"
#include <map>

namespace controller {  // <namespace controller>

class SimpleChannelListener : public ChannelListener {
 public:
  // ChannelListener overrides.
  void onChannelUp(Channel *channel) override;
  void onMessage(const Message *message) override;

  // Forwards reply to correct handler for xid and removes xid entry.
  void onReply(const Message *message);

  // Callbacks for replies we expect to receive during setup.
  void onGetAsyncReply(const GetAsyncReply *msg);
  void onGetConfigReply(const GetConfigReply *msg);
  void onBarrierReply(const BarrierReply *msg);

 private:
  // Our controller object handles PacketIn messages.
  SimpleController controller_;

  // Implement a simple reply tracker that will allow us to register callbacks
  // for our requests. The reply callbacks will be pointers to member
  // functions.
  using Tracker = std::map<UInt32, std::function<void(const Message *message)>>;

  // Our tracker to map xid -> handler function.
  Tracker tracker_;

  // Typedef for pointer to a member function in this class.
  template <class MesgType>
  using PtrMemFunc = void (SimpleChannelListener::*)(const MesgType *msg);

  // Adds our typed member function to the tracker map for the specified xid.
  template <class MesgType>
  void trackReply(UInt32 xid, PtrMemFunc<MesgType> mbf);
};

template <class MesgType>
void SimpleChannelListener::trackReply(UInt32 xid, PtrMemFunc<MesgType> mbf) {
  auto handler = [this, mbf](const Message *message) {
    if (message->type() == MesgType::type()) {
      if (const MesgType *msg = MesgType::cast(message)) {
        // Call pointer to member function.
        (this->*mbf)(msg);
      }
    } else {
      log::debug("Unexpected message type:", message->type());
    }
  };

  auto item = tracker_.insert(std::make_pair(xid, handler));
  if (!item.second) {
    log::debug("XID already in use?");
  }
}

}  // </namespace controller>
