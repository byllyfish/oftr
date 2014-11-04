// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/api/apiconnectionsession.h"
#include "ofp/sys/engine.h"
#include "ofp/api/apisession.h"

using ofp::api::ApiConnectionSession;

ApiConnectionSession::ApiConnectionSession(ApiServer *server,
                                           ApiSession *session)
    : ApiConnection{server}, session_{session}, work_{server->engine()->io()} {}

void ApiConnectionSession::write(const std::string &msg) {
  session_->receive(msg);
}

void ApiConnectionSession::asyncRead() {}
