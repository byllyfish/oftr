#ifndef OFP_CHANNELMODE_H_
#define OFP_CHANNELMODE_H_

namespace ofp {

enum class ChannelMode {
    Agent = 0,
    Controller,
    Bridge,
    Auxiliary
};

}  // namespace ofp

#endif // OFP_CHANNELMODE_H_
