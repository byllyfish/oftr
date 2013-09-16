#include "ofp/yaml/yflowmod.h"

namespace ofp {  // <namespace ofp>
namespace yaml { // <namespace yaml>

#if 0
template <>
Exception read(const std::string &input, FlowModBuilder *msg)
{
    llvm::yaml::Input yin(input);

    detail::FlowModBuilderWrap wrap{msg};
    yin >> wrap;

    if (yin.error()) {
        return Exception{}; // FIXME
    }

    return Exception{};
}
#endif //0

//
// Encode a FlowMod message as follows:
//
//      ---
//      version: $header.version()
//      type: $header.type()
//      xid: $header.xid()
//      msg:
//          cookie: $cookie_
//          cookie_mask: $cookieMask_
//          table_id: $tableId_
//          command: $command_
//          idle_timeout: $idleTimeout_
//          hard_timeout: $hardTimeout_
//          priority: $priority_
//          buffer_id: $bufferId_
//          out_port: $outPort_
//          out_group: $outGroup_
//          flags: $flags_
//          match:
//              - type: OFB_IN_PORT
//                value: 5
//          instructions:
//              - type: GOTO_TABLE
//                value: 5
//              - type: WRITE_ACTIONS
//                  - type: AT_SET_NW_TTL
//                    value: 6
//                  - type: AT_OUTPUT
//                    value:
//                      - port: 2
//                      - maxlen: 70
//      ...

#if 0
template <>
std::string write(const FlowMod *msg)
{
    std::string result;
    llvm::raw_string_ostream rss{result};
    llvm::yaml::Output yout{rss};

    detail::FlowModWrap wrap{msg};
    yout << wrap;
    return rss.str();
}
#endif //0

} // </namespace yaml>
} // </namespace ofp>