#include "ofp/unittest.h"
#include "ofp/oxmvalue.h"
#include "ofp/oxmiterator.h"
#include "ofp/oxmrange.h"

using namespace ofp;

namespace {

constexpr OXMInternalID cast(int n) { return static_cast<OXMInternalID>(n); }

using ofb_in_port = OXMValue<cast(0), 0x8000, 0, Big16, 2, false>;
using ofb_vlan_vid = OXMValue<cast(1), 0x8000, 6, Big16, 2, true>;
using ofb_tcp_src_port = OXMValue<cast(2), 0x8000, 19, Big16, 2, false>;

const char *buffer = "8000 0002 0100"
                     "8000 0c02 FFFF"
                     "8000 2602 0064";
}

TEST(oxmiterator, test)
{
    auto buf = HexToRawData(buffer);

    OXMRange data{buf.data(), buf.size()};
    OXMIterator begin = data.begin();
    OXMIterator end = data.end();

    for (auto iter = begin; iter != end; ++iter) {
        const OXMIterator::Item &item = *iter;
        
        switch (item.type()) {
        case ofb_tcp_src_port::type() : {
            auto srcPort = item.value<ofb_tcp_src_port>();
            EXPECT_EQ(100, srcPort);
            break;
        }

        case ofb_vlan_vid::type() : {
            auto vlan = item.value<ofb_vlan_vid>();
            EXPECT_EQ(0xFFFF, vlan);
            break;
        }

        case ofb_in_port::type() : {
            auto port = item.value<ofb_in_port>();
            EXPECT_EQ(256, port);
        } break;

        default:
            EXPECT_TRUE(false);
            break;
        }
    }
}
