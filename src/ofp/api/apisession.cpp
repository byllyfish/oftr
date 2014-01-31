#include "ofp/api/apisession.h"
#include "ofp/api/apiconnectionsession.h"
#include "ofp/sys/engine.h"

using namespace ofp::api;

void ApiSession::send(const std::string &msg) {
    asio::io_service &io = driver_.engine()->io();

    auto conn = conn_;
    io.post([conn,msg](){
        conn->handleEvent(msg);
    });
}

void ApiSession::setConnection(const std::shared_ptr<ApiConnectionSession> &conn)
{
    assert(conn_.use_count() == 0);

    conn_ = conn;
}
