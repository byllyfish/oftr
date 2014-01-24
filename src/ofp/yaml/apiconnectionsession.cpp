#include "ofp/yaml/apiconnectionsession.h"
#include "ofp/sys/engine.h"
#include "ofp/yaml/apisession.h"

using namespace ofp::yaml;

ApiConnectionSession::ApiConnectionSession(ApiServer *server, ApiSession *session)
    : ApiConnection{server, false}, session_{session}, work_{server->engine()->io()}
{
}

void ApiConnectionSession::write(const std::string &msg) {
     session_->receive(msg);
}

void ApiConnectionSession::asyncRead() {

}
