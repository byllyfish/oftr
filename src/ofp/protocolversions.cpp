#include "ofp/protocolversions.h"

ofp::ProtocolVersions::ProtocolVersions() : bitmap_{All}
{
}

ofp::ProtocolVersions::ProtocolVersions(Setting setting) : bitmap_{setting}
{
}

ofp::ProtocolVersions::ProtocolVersions(std::initializer_list<UInt8> versions)
{
    for (auto v : versions) {
        assert(v <= MaxVersion);
        if (v <= MaxVersion)
            bitmap_ |= (1 << v);
    }
}

bool ofp::ProtocolVersions::empty() const
{
	return (bitmap_ == 0);
}

ofp::UInt8 ofp::ProtocolVersions::highestVersion() const
{
    UInt32 bits = bitmap_;
    for (unsigned i = MaxVersion; i > 0; --i) {
        if ((bits >> i) & 1) {
            return UInt8_narrow_cast(i);
        }
    }
    return 0;
}

ofp::UInt32 ofp::ProtocolVersions::bitmap() const
{
    return bitmap_;
}


ofp::ProtocolVersions ofp::ProtocolVersions::intersection(ProtocolVersions versions) const
{
	return fromBitmap(bitmap_ & versions.bitmap_);
}


ofp::ProtocolVersions ofp::ProtocolVersions::fromBitmap(UInt32 bitmap)
{
    ProtocolVersions versions;
    versions.bitmap_ = bitmap;
    return versions;
}
