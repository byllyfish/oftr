#ifndef OFP_PORTMOD_H
#define OFP_PORTMOD_H

#include "ofp/protocolmsg.h"
#include "ofp/padding.h"

namespace ofp { // <namespace ofp>

class PortMod : public ProtocolMsg<PortMod,OFPT_PORT_MOD> {
public:

	UInt32 portNo() const { return portNo_; }
	EnetAddress hwAddr() const { return hwAddr_; }
	UInt32 config() const { return config_; }
	UInt32 mask() const { return mask_; }
	UInt32 advertise() const { return advertise_; }

	bool validateLength(size_t length) const;

private:
	Header header_;
	Big32 portNo_;
	Padding<4> pad1_;
	EnetAddress hwAddr_;
	Padding<2> pad2_;
	Big32 config_;
	Big32 mask_;
	Big32 advertise_;
	Padding<4> pad3_;

    // Only PortModBuilder can construct an actual instance.
    PortMod() : header_{type()}
    {
    }

	friend class PortModBuilder;
	template <class T>
    friend struct llvm::yaml::MappingTraits;
};


static_assert(sizeof(PortMod) == 40, "Unexpected size.");
static_assert(IsStandardLayout<PortMod>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<PortMod>(), "Expected trivially copyable.");


class PortModBuilder {
public:
	PortModBuilder() = default;
	explicit PortModBuilder(const PortMod *msg);

	void setPortNo(UInt32 portNo) { msg_.portNo_ = portNo; }
	void setHwAddr(const EnetAddress &hwAddr) { msg_.hwAddr_ = hwAddr; }
	void setConfig(UInt32 config) { msg_.config_ = config; }
	void setMask(UInt32 mask) { msg_.mask_ = mask; }
	void setAdvertise(UInt32 advertise) { msg_.advertise_ = advertise; }

	UInt32 send(Writable *channel);

private:
	PortMod msg_;

	template <class T>
    friend struct llvm::yaml::MappingTraits;
};

} // </namespace ofp>

#endif // OFP_PORTMOD_H