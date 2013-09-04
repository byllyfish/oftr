#include "ofp/yaml/ysetconfig.h"


namespace ofp {  // <namespace ofp>
namespace yaml { // <namespace yaml>


// FIXME - these are HeaderOnly messages... put these in yheaderonly.cpp

template <>
Exception read(const std::string &input, SetConfigBuilder *msg)
{
    llvm::yaml::Input yin(input);
    yin >> *msg;

    if (yin.error()) {
        return Exception{}; // FIXME
    }

    return Exception{};
}

template <>
std::string write(const SetConfig *msg)
{
    std::string result;
    llvm::raw_string_ostream rss{result};
    llvm::yaml::Output yout{rss};

    yout << ofp::detail::YamlRemoveConst_cast(*msg);
    return rss.str();
}

} // </namespace yaml>
} // </namespace ofp>