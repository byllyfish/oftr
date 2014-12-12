
#include <xpc/xpc.h>            // Mac OS X
#include "ofp/log.h"
#include "ofp/api/apisession.h"
#include "ofpx_xpc.h"

static void ofpx_xpc_event_handler(xpc_connection_t peer);
static void ofpx_xpc_peer_event_handler(xpc_connection_t peer, xpc_object_t event);
static void *ofpx_xpc_server_thread(void *context);

class XpcSession : public ofp::api::ApiSession {
public:
    XpcSession(xpc_connection_t peer) : peer_{peer} {}
    
    virtual void receive(const std::string &msg) {
        xpc_object_t dict = xpc_dictionary_create(NULL, NULL, 0);
        xpc_dictionary_set_data(dict, "data", msg.data(), msg.size());
        xpc_connection_send_message(peer_, dict);
        xpc_release(dict);
    }
    
    static XpcSession *obtainFromPeer(xpc_connection_t conn) {
        return static_cast<XpcSession *>(xpc_connection_get_context(conn));
    }
    
private:
    xpc_connection_t peer_;
};



void ofpx::run_xpc_main(void) {
    xpc_main(ofpx_xpc_event_handler);
}

void ofpx_xpc_event_handler(xpc_connection_t peer)
{
    // The peer event handlers runs on the main thread. The event handlers
    // simply pass messages to and from our own server thread using the
    // libofp ApiSession protocol.

    dispatch_queue_t mainQueue = dispatch_get_main_queue();
    xpc_connection_set_target_queue(peer, mainQueue);
    xpc_connection_set_event_handler(peer, ^(xpc_object_t event) {
        ofpx_xpc_peer_event_handler(peer, event);
    });
    
    // Create a second thread to run the server.
    // After creating the server, we set the xpc_connection's context to an
    // object that lets us send events to the server thread and receive events
    // back.
    // When the server is ready to accept events, the server thread will call
    // xpc_connection_resume() to tell the connection to begin listening for
    // events.
    
    xpc_retain(peer);
    
    pthread_t thread;
    int err = pthread_create(&thread, NULL, ofpx_xpc_server_thread, peer);
    if (!err) {
        err = pthread_detach(thread);
    }
    
    if (err) {
        // Problem creating a thread? Close the connection.
        xpc_release(peer);
        xpc_connection_cancel(peer);
    }
}


void ofpx_xpc_peer_event_handler(xpc_connection_t peer, xpc_object_t event)
{
    static bool boosted = false;

    if (!boosted) {
        xpc_transaction_begin();
        boosted = true;
    }
    
    XpcSession *session = XpcSession::obtainFromPeer(peer);
    xpc_type_t type = xpc_get_type(event);

    if (type == XPC_TYPE_ERROR) {
        const char *errorStr = xpc_dictionary_get_string(event, XPC_ERROR_KEY_DESCRIPTION);
        ofp::log::error("ofpx_xpc_peer_event_handler: error", errorStr);
        
        if (session) {
            ofp::log::info("ofpx_xpc_peer_event_handler: stopping session");
            xpc_connection_set_context(peer, nullptr);
            session->stop();
        }

        return;    
    }

    assert(type == XPC_TYPE_DICTIONARY);
    assert(session);

    size_t len = 0;
    const void *data = xpc_dictionary_get_data(event, "data", &len);

    if (data == NULL || len == 0) {
        ofp::log::info("ofpx_xpc_peer_event_handler: no message found");
    } else {
        const char *s = static_cast<const char *>(data);
        std::string msg{s, len};
        session->send(msg);
        ofp::log::info("ofpx_xpc_peer_event_handler: send:\n%s", msg.c_str());
    }
}


void *ofpx_xpc_server_thread(void *context)
{
    xpc_connection_t peer = static_cast<xpc_connection_t>(context);

    XpcSession session{peer};
    xpc_connection_set_context(peer, &session);
    xpc_connection_resume(peer);
    session.run();
    
    xpc_release(peer);
    return nullptr;
}




