#include "ofp/unittest.h"
#include "ofp/rpc/filteractiongenericreply.h"
#include "ofp/message.h"
#include "ofp/yaml/encoder.h"
#include "ofp/yaml/decoder.h"
#include "ofp/mockchannel.h"
#include "ofp/packetin.h"

using namespace ofp;
using namespace ofp::rpc;


static Message encodeMessage(llvm::StringRef yaml) {
  ofp::yaml::Encoder encoder{yaml};
  assert(encoder.error() == "");
  return Message{encoder.data(), encoder.size()};
}

static std::string decodeMessage(ByteRange data) {
  log_info("decodeMessage", RawDataToHex(data.data(), data.size()));
  Message message{data.data(), data.size()};
  message.normalize();

  ofp::yaml::Decoder decoder{&message, false, true};
  return decoder.result();
}


TEST(rpcfilteractiongenericreply, test) {
  const char *input = R"""(
    type:            PACKET_IN
    xid:             0x00000000
    version:         0x04
    msg:             
      buffer_id:       NO_BUFFER
      total_len:       0x05EE
      in_port:         0x00000001
      in_phy_port:     0x00000001
      metadata:        0x0000000000000000
      reason:          APPLY_ACTION
      table_id:        0x06
      cookie:          0x00000000FFFFFFFF
      match:           
        - field:           IN_PORT
          value:           0x00000001
      data:            0E00000000010AC2BB024296810000640800450005DC0518200040013AA70A0000010A6400FE080014572C2400059DA8FC590000000046D5060000000000101112131415161718191A1B1C1D1E1F202122232425
  )""";

  Message message = encodeMessage(input);
  ASSERT_TRUE(message.type() == OFPT_PACKET_IN);
  message.normalize();

  MockChannel outputChannel{OFP_VERSION_4};
  message.setSource(&outputChannel);

  const PacketIn *packetIn = PacketIn::cast(&message);
  ASSERT_TRUE(packetIn != nullptr);
  ByteRange enetFrame = packetIn->enetFrame();
  PortNumber inPort = packetIn->inPort();
  
  FilterActionGenericReply action; 
  bool result = action.apply(enetFrame, inPort, &message);
  EXPECT_TRUE(result);

  std::string output = decodeMessage({outputChannel.data(), outputChannel.size()});
  EXPECT_EQ(output, "---\ntype:            PACKET_OUT\nxid:             0x00000001\nversion:         0x04\nmsg:             \n  buffer_id:       NO_BUFFER\n  in_port:         CONTROLLER\n  actions:         \n    - action:          OUTPUT\n      port_no:         0x00000001\n      max_len:         0x0000\n  data:            0AC2BB0242960E00000000018100006408004500004200000000400165590A6400FE0A0000010000CDD42C2400059DA8FC590000000046D5060000000000101112131415161718191A1B1C1D1E1F202122232425\n  _pkt:            \n    - field:           ETH_DST\n      value:           '0a:c2:bb:02:42:96'\n    - field:           ETH_SRC\n      value:           '0e:00:00:00:00:01'\n    - field:           VLAN_VID\n      value:           0x1064\n    - field:           VLAN_PCP\n      value:           0x00\n    - field:           ETH_TYPE\n      value:           0x0800\n    - field:           IP_DSCP\n      value:           0x00\n    - field:           IP_ECN\n      value:           0x00\n    - field:           IP_PROTO\n      value:           0x01\n    - field:           IPV4_SRC\n      value:           10.100.0.254\n    - field:           IPV4_DST\n      value:           10.0.0.1\n    - field:           NX_IP_TTL\n      value:           0x40\n    - field:           ICMPV4_TYPE\n      value:           0x00\n    - field:           ICMPV4_CODE\n      value:           0x00\n    - field:           X_PKT_POS\n      value:           0x002A\n...\n");
}
