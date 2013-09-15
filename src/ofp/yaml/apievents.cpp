#include "ofp/yaml/apievents.h"

using namespace ofp::yaml;

namespace { // <namespace>

/// Converts event to a YAML representation.
template <class Type>
std::string convertToString(Type *event)
{
	std::string yaml;
	llvm::raw_string_ostream rss(yaml);
    llvm::yaml::Output yout{rss};
    yout << *event;
    (void)rss.str();
	return yaml;
}

} // </namespace>


/// \returns YAML representation of LIBOFP_LISTEN_REPLY event.
std::string ApiListenReply::toString()
{
	return convertToString(this);
}

/// \returns YAML representation of LIBOFP_YAML_ERROR event.
std::string ApiYamlError::toString()
{
	return convertToString(this);
}

/// \returns YAML representation of LIBOFP_DECODE_ERROR event.
std::string ApiDecodeError::toString()
{
	return convertToString(this);
}

/// \returns YAML representation of LIBOFP_DATAPATH_UP event.
std::string ApiDatapathUp::toString()
{
	return convertToString(this);
}

/// \returns YAML representation of LIBOFP_DATAPATH_DOWN event.
std::string ApiDatapathDown::toString()
{
	return convertToString(this);
}

/// \returns YAML representation of LIBOFP_TIMER event.
std::string ApiTimer::toString()
{
	return convertToString(this);
}