#include "ofp/unittest.h"
#include "ofp/oxmrange.h"
#include "ofp/oxmvalue.h"

using namespace ofp;

namespace {

constexpr OXMInternalID cast(int n) { return static_cast<OXMInternalID>(n); }

using OFB_InPort = OXMValue<cast(0), 0x8000, 0, Big16, 2, false>;
using OFB_VlanVid = OXMValue<cast(1), 0x8000, 6, Big16, 2, true>;
using OFB_TCPSrcPort = OXMValue<cast(2), 0x8000, 19, Big16, 2, false>;

const char *buffer = "8000 0002 0100"
                     "8000 0c02 FFFF"
                     "8000 2602 0064";
}

TEST(oxmrange, test) {
  auto buf = HexToRawData(buffer);
  OXMRange range{buf.data(), buf.size()};

  UInt16 port = 0;
  for (auto &item : range) {
    switch (item.type()) {
    case OFB_TCPSrcPort::type() :
      port = item.value<OFB_TCPSrcPort>();
      break;
    }
  }

  EXPECT_EQ(100, port);
}

TEST(oxmrange, validateInput_invalid_1) {
  auto buf = HexToRawData("8000050FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF8000090CFFFFFFFFFFFFFFFFFFFFFFFF80000806FFFFFFFFFFFF80000510FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF80000D04FFFFFFFF80000D04FFFFFFFF");
  OXMRange range{buf.data(), buf.size()};

  EXPECT_FALSE(range.validateInput());
}
