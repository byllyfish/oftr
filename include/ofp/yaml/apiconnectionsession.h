#ifndef OFP_YAML_APICONNECTIONSESSION_H
#define OFP_YAML_APICONNECTIONSESSION_H

#include "ofp/sys/asio_utils.h"
#include "ofp/yaml/apiconnection.h"

namespace ofp { // <namespace ofp>
namespace yaml { // <namespace yaml>

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

} // </namespace yaml>
} // </namespace ofp>

#endif // OFP_YAML_APICONNECTIONSESSION_H
