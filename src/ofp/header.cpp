#include "ofp/header.h"

namespace ofp { // <namespace ofp>

static UInt8 MaxTypeByVersion[] = {0,
                                   UInt8_cast(deprecated::v1::OFPT_LAST),
                                   UInt8_cast(deprecated::v2::OFPT_LAST),
                                   UInt8_cast(deprecated::v3::OFPT_LAST),
                                   UInt8_cast(OFPT_LAST)};

static UInt8 translateTypeToVersion(UInt8 type, UInt8 version)
{
    assert(type > OFPT_FLOW_MOD);
    assert(version <= OFP_VERSION_3);

    if (version == OFP_VERSION_1) {
        if (type == OFPT_GROUP_MOD || type == OFPT_TABLE_MOD)
            return OFPT_UNSUPPORTED;
        type = (type == OFPT_PORT_MOD) ? type - 1 : type - 2;
    }

    if (type > MaxTypeByVersion[version]) {
        return OFPT_UNSUPPORTED;
    }

    return type;
}

static UInt8 translateTypeFromVersion(UInt8 type, UInt8 version)
{
    assert(type > OFPT_FLOW_MOD);
    assert(version <= OFP_VERSION_3);

    if (type > MaxTypeByVersion[version]) {
        return OFPT_UNSUPPORTED;
    }

    if (version == OFP_VERSION_1) {
        if (type == UInt8_cast(deprecated::v1::OFPT_PORT_MOD)) {
            type = OFPT_PORT_MOD;
        } else if (type > UInt8_cast(deprecated::v1::OFPT_PORT_MOD)) {
            type += 2;
        }
    }

    return type;
}

UInt8 Header::translateType(UInt8 version, UInt8 type, UInt8 newVersion)
{
    assert(version >= OFP_VERSION_1);

    if (version == newVersion)
        return type;

    assert(version == OFP_VERSION_4 || newVersion == OFP_VERSION_4);

    if (version > OFP_VERSION_4 || newVersion > OFP_VERSION_4)
        return type;

    if (type <= OFPT_FLOW_MOD)
        return type;

    if (version == OFP_VERSION_4) {
        return translateTypeToVersion(type, newVersion);

    } else if (newVersion == OFP_VERSION_4) {
        return translateTypeFromVersion(type, version);

    } else {
        return OFPT_UNSUPPORTED;
    }
}

} // </namespace ofp>