#include "ofp/sys/udp_server.h"
#include "ofp/sys/udp_connection.h"
#include "ofp/sys/engine.h"
#include "ofp/echorequest.h"
#include "ofp/echoreply.h"
#include "ofp/hello.h"

using namespace boost;


ofp::sys::UDP_Server::UDP_Server(Engine *engine, Driver::Role role, const udp::endpoint &endpt, ProtocolVersions versions)
    : engine_{engine}, role_{role}, versions_{versions}, socket_{engine->io(), endpt}, message_{nullptr}
{
    asyncReceive();
}


void ofp::sys::UDP_Server::add(UDP_Connection *conn){
	connMap_.insert(std::make_pair(conn->remoteEndpoint(), conn));
}

void ofp::sys::UDP_Server::remove(UDP_Connection *conn) {

	if (!shuttingDown_) {
		auto iter = connMap_.find(conn->remoteEndpoint());
		if (iter != connMap_.end()) {
			connMap_.erase(iter);

		} else {
			log::error("UDP_Server::remove - cannot find connection");
		}
	}
}


void ofp::sys::UDP_Server::write(const void *data, size_t length)
{
    // write to buffer
}

void ofp::sys::UDP_Server::flush(udp::endpoint endpt)
{
    asyncSend();
}


void ofp::sys::UDP_Server::asyncReceive()
{
    log::debug(__PRETTY_FUNCTION__);
    
    socket_.async_receive_from(
        asio::buffer(message_.mutableData(MaxDatagramLength), MaxDatagramLength),
        sender_, [this](error_code err, size_t bytes_recvd) {

        if (err) {
            log::info("Error receiving datagram:", makeException(err));

        } else if (bytes_recvd < sizeof(Header)) {
        	log::info("Small datagram ignored:", bytes_recvd);

        } else if (message_.header()->length() != bytes_recvd) {
        	log::info("Mismatch between datagram size and header length:", message_);
        
        } else {
    		message_.shrink(bytes_recvd);
        	dispatchMessage();
        }

        asyncReceive();
    });
}

void ofp::sys::UDP_Server::asyncSend()
{

}

void ofp::sys::UDP_Server::dispatchMessage()
{
    log::debug("Receive datagram:", message_);

    // If the message is an EchoRequest, reply immediately; it doesn't matter
    // if there is an existing connection or not.
    
    if (message_.type() == EchoRequest::Type) {
    	auto request = EchoRequest::cast(&message_);
    	if (request) {
            message_.setType(EchoReply::Type);
            write(message_.data(), message_.size());
            flush(sender_);
    	} else {
            log::info("Invalid EchoRequest dropped.");
        }
    	return;
    }

    // Lookup sender_ to find an existing UDP_Connection. If it exists, dispatch
    // incoming message to that connection. If there is no related connection
    // and the incoming message is a HELLO, open a new connection. Otherwise, 
    // drop the message.

    auto iter = connMap_.find(sender_);

    if (iter == connMap_.end()) {
    	
    	if (message_.type() == Hello::Type) {
    		auto conn = new UDP_Connection(this, role_, versions_, sender_);
    		conn->postMessage(nullptr, &message_);

    	} else {
    		log::info("Unexpected message ignored: ", message_);
    	}
    	
    } else {
    	// Dispatch incoming message to existing connection.
    	iter->second->postMessage(nullptr, &message_);
    }
}

