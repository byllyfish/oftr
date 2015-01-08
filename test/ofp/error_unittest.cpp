#include "ofp/unittest.h"
#include "ofp/error.h"

using namespace ofp;

TEST(error, experimenter) {
    ErrorBuilder errorBuilder{0x11111111};

    errorBuilder.setErrorTypeEnum(OFPET_EXPERIMENTER);
    errorBuilder.setErrorCode(1);

    MemoryChannel channel{OFP_VERSION_1};
    errorBuilder.send(&channel);

    Message message{channel.data(), channel.size()};
    message.transmogrify();
    EXPECT_HEX("0101000C00000001FFFF0001", message.data(), message.size());

    const Error *error = Error::cast(&message);

    EXPECT_EQ(0xffff, error->errorType());
    EXPECT_EQ(1, error->errorCode());

    EXPECT_EQ(OFPET_EXPERIMENTER, error->errorTypeEnum());
}


TEST(error, flowmod_failed_v1) {
    ErrorBuilder errorBuilder{0x11111111};

    errorBuilder.setErrorTypeEnum(OFPET_FLOW_MOD_FAILED);
    errorBuilder.setErrorCodeEnum(OFPFMFC_TABLE_FULL);

    MemoryChannel channel{OFP_VERSION_1};
    errorBuilder.send(&channel);

    Message message{channel.data(), channel.size()};
    message.transmogrify();
    EXPECT_HEX("0101000C0000000100030000", message.data(), message.size());

    const Error *error = Error::cast(&message);

    EXPECT_EQ(3, error->errorType());
    EXPECT_EQ(0, error->errorCode());

    EXPECT_EQ(OFPET_FLOW_MOD_FAILED, error->errorTypeEnum());
    EXPECT_EQ(OFPFMFC_TABLE_FULL, error->errorCodeEnum());
}

TEST(error, flowmod_failed_v4) {
    ErrorBuilder errorBuilder{0x11111111};

    errorBuilder.setErrorTypeEnum(OFPET_FLOW_MOD_FAILED);
    errorBuilder.setErrorCodeEnum(OFPFMFC_TABLE_FULL);

    MemoryChannel channel{OFP_VERSION_4};
    errorBuilder.send(&channel);

    Message message{channel.data(), channel.size()};
    message.transmogrify();
    EXPECT_HEX("0401000C0000000100050001", message.data(), message.size());

    const Error *error = Error::cast(&message);

    EXPECT_EQ(5, error->errorType());
    EXPECT_EQ(1, error->errorCode());

    EXPECT_EQ(OFPET_FLOW_MOD_FAILED, error->errorTypeEnum());
    EXPECT_EQ(OFPFMFC_TABLE_FULL, error->errorCodeEnum());
}

TEST(error, unsupported_order_v1) {
    ErrorBuilder errorBuilder{0x11111111};

    errorBuilder.setErrorTypeEnum(OFPET_BAD_ACTION);
    errorBuilder.setErrorCodeEnum(OFPBAC_UNSUPPORTED_ORDER);

    MemoryChannel channel{OFP_VERSION_1};
    errorBuilder.send(&channel);

    Message message{channel.data(), channel.size()};
    message.transmogrify();
    EXPECT_HEX("0101000C0000000100020005", message.data(), message.size());

    const Error *error = Error::cast(&message);

    EXPECT_EQ(OFP_VERSION_1, error->msgHeader()->version());
    EXPECT_EQ(OFPET_BAD_ACTION, error->errorType());
    EXPECT_EQ(5, error->errorCode());

    EXPECT_EQ(OFPET_BAD_ACTION, error->errorTypeEnum());
    EXPECT_EQ(OFPBAC_UNSUPPORTED_ORDER, error->errorCodeEnum());
}

TEST(error, unsupported_order_v4) {
    ErrorBuilder errorBuilder{0x11111111};

    errorBuilder.setErrorTypeEnum(OFPET_BAD_ACTION);
    errorBuilder.setErrorCodeEnum(OFPBAC_UNSUPPORTED_ORDER);

    MemoryChannel channel{OFP_VERSION_4};
    errorBuilder.send(&channel);

    Message message{channel.data(), channel.size()};
    message.transmogrify();
    EXPECT_HEX("0401000C000000010002000B", message.data(), message.size());

    const Error *error = Error::cast(&message);

    EXPECT_EQ(OFP_VERSION_4, error->msgHeader()->version());
    EXPECT_EQ(OFPET_BAD_ACTION, error->errorType());
    EXPECT_EQ(OFPBAC_UNSUPPORTED_ORDER & 0xffff, error->errorCode());

    EXPECT_EQ(OFPET_BAD_ACTION, error->errorTypeEnum());
    EXPECT_EQ(OFPBAC_UNSUPPORTED_ORDER, error->errorCodeEnum());
}
