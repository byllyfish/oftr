#include "ofp/yaml/apisession.h"
#include "ofp/yaml/apiconnectionsession.h"
#include "ofp/sys/engine.h"

using namespace ofp::yaml;

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
