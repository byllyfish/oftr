#include "ofp/yaml/apiconnectionstdio.h"
#include "ofp/sys/boost_asio.h"

using namespace ofp::yaml;
using namespace ofp::sys;

ApiConnectionStdio::ApiConnectionStdio(ApiServer *server,
                                       sys::stream_descriptor input,
                                       sys::stream_descriptor output)
    : ApiConnection{server}, input_{std::move(input)},
      output_{std::move(output)}
{
}

void ApiConnectionStdio::write(const std::string &msg)
{
    outgoing_[outgoingIdx_].add(msg.data(), msg.length());
    if (!writing_) {
        asyncWrite();
    }
}

void ApiConnectionStdio::asyncAccept()
{
    // Start first async read.
    asyncRead();
}

void ApiConnectionStdio::asyncRead()
{
    auto self(shared_from_this());

    boost::asio::async_read_until(
        input_, streambuf_, '\n',
        [this, self](const error_code & err, size_t bytes_transferred) {
            if (!err) {
                std::istream is(&streambuf_);
                std::string line;
                std::getline(is, line);
                handleInputLine(&line);
                asyncRead();
            } else if (!isAsioEOF(err)) {
                auto exc = makeException(err);
                log::info("ApiConnection::asyncRead err", exc);
            }
        });
}

void ApiConnectionStdio::asyncWrite()
{
    assert(!writing_);

    int idx = outgoingIdx_;
    outgoingIdx_ = !outgoingIdx_;
    writing_ = true;

    const UInt8 *data = outgoing_[idx].data();
    size_t size = outgoing_[idx].size();

    auto self(shared_from_this());

    boost::asio::async_write(
        output_, boost::asio::buffer(data, size),
        [this, self](const error_code & err, size_t bytes_transferred) {

            if (!err) {
                assert(bytes_transferred == outgoing_[!outgoingIdx_].size());

                writing_ = false;
                outgoing_[!outgoingIdx_].clear();
                if (outgoing_[outgoingIdx_].size() > 0) {
                    // Start another async write for the other output buffer.
                    asyncWrite();
                }

            } else {
                log::debug("Write error ", makeException(err));
            }
        });
}
