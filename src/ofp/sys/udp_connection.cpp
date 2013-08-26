#include "ofp/sys/udp_connection.h"
#include "ofp/defaulthandshake.h"
#include "ofp/sys/udp_server.h"
#include "ofp/log.h"


ofp::sys::UDP_Connection::UDP_Connection(UDP_Server *server, Driver::Role role, ProtocolVersions versions, udp::endpoint remoteEndpt) 
: Connection{server->engine(), new DefaultHandshake{this, role, versions, nullptr}}, server_{server}, remoteEndpt_{remoteEndpt}
{
	server_->add(this);
}

ofp::sys::UDP_Connection::~UDP_Connection() {
	server_->remove(this);
}

void ofp::sys::UDP_Connection::write(const void *data, size_t length) {
	server_->write(data, length);
}

void ofp::sys::UDP_Connection::flush() {
	server_->flush(remoteEndpt_);
}

void ofp::sys::UDP_Connection::close() 
{
	log::info("close the 'connection' to this udp endpoint");

	// call delete this?
}
