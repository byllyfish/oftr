// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_API_APISESSION_H_
#define OFP_API_APISESSION_H_

#include "ofp/api/apiserver.h"

namespace ofp {
namespace api {

class ApiConnectionSession;

/// Communicate with an ApiServer running in another thread. Subclass to
/// implement receive() to receive data from the thread (receive() is called
/// from the server thread.)

class ApiSession {
 public:
  ApiSession() : server_{&driver_, this} {}
  virtual ~ApiSession() {}

  void run() { driver_.run(); }
  void stop() { driver_.stop(); }

  void send(const std::string &msg);
  virtual void receive(const std::string &msg) = 0;

  void setConnection(const std::shared_ptr<ApiConnectionSession> &conn);

 private:
  Driver driver_;
  // N.B. conn_ is used in initialization of server_. conn_ also depends on
  // driver_, so it must be initialized after driver_.
  std::shared_ptr<ApiConnectionSession> conn_;
  ApiServer server_;
};

}  // namespace api
}  // namespace ofp

#endif  // OFP_API_APISESSION_H_
