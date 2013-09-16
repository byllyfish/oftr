#ifndef OFP_YAML_APICONNECTION_H
#define OFP_YAML_APICONNECTION_H

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/apiserver.h"
#include "ofp/bytelist.h"

OFP_BEGIN_IGNORE_PADDING

namespace ofp { // <namespace ofp>
namespace yaml { // <namespace yaml>

struct ApiListenRequest;
struct ApiSetTimer;

class ApiConnection : public std::enable_shared_from_this<ApiConnection> {
public:
	 ApiConnection(ApiServer *server, sys::tcp::socket socket);
	~ApiConnection();

	void asyncAccept();

	void onListenRequest(ApiListenRequest *listenReq);
	void onListenReply(ApiListenReply *listenReply);
	void onSetTimer(ApiSetTimer *setTimer);

	void onYamlError(const std::string &error, const std::string &text);
	void onChannelUp(Channel *channel);
	void onChannelDown(Channel *channel);
	void onMessage(Channel *channel, const Message *message);
	void onException(Channel *channel, const Exception *exception);
	void onTimer(Channel *channel, UInt32 timerID);

	void write(const std::string &msg);

private:
	ApiServer *server_;
	sys::tcp::socket socket_;
	boost::asio::streambuf streambuf_;
	std::string text_;
	unsigned lineCount_ = 0;
	bool isLibEvent_ = false;

	// Use a two buffer strategy for async-writes. We queue up data in one
	// buffer while we're in the process of writing the other buffer.
	ByteList outgoing_[2];
	int outgoingIdx_ = 0;
	bool writing_ = false;
	
	void asyncRead();
	void asyncWrite();

	void handleInputLine(const std::string &line);
	void handleEvent();

	static void cleanInputLine(std::string *line);
	static bool isEmptyEvent(const std::string &s);
	static std::string escape(const std::string &s);
};

} // </namespace yaml>
} // </namespace ofp>

OFP_END_IGNORE_PADDING

#endif // OFP_YAML_APICONNECTION_H
