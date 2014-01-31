#ifndef OFP_API_APICONNECTIONSESSION_H
#define OFP_API_APICONNECTIONSESSION_H

#include "ofp/sys/asio_utils.h"
#include "ofp/api/apiconnection.h"

namespace ofp { // <namespace ofp>
namespace api { // <namespace api>

OFP_BEGIN_IGNORE_PADDING

class ApiConnectionSession : public ApiConnection {
public:
    ApiConnectionSession(ApiServer *server, ApiSession *session);

    void asyncAccept() override {}

protected:
    void write(const std::string &msg) override;
    void asyncRead() override;

private:
    ApiSession *session_;
    asio::io_service::work work_;
};

OFP_END_IGNORE_PADDING

} // </namespace api>
} // </namespace ofp>

#endif // OFP_API_APICONNECTIONSESSION_H
