#include "ofp/unittest.h"
#include "ofp/yaml/flowmod.h"
#include "ofp/writable.h"

using namespace ofp;


class WriteToByteList : public Writable {
public:
	WriteToByteList(UInt8 version, UInt32 xid, ByteList *data): data_{data}, xid_{xid}, version_{version}  {}

	UInt8 version() const override { return version_; }
	UInt32 nextXid() override { return xid_; }
	void write(const void *data, size_t length) override {
		data_->add(data, length);
	}
	void flush() override {}

private:
	ByteList *data_;
	UInt32 xid_;
	UInt8 version_;
};


template <class MesgBuilderType>
void serialize(MesgBuilderType &msg, ByteList *data)
{
	WriteToByteList writer{4, 1, data};
	msg.send(&writer);
}


TEST(yaml_flowmod, test) 
{
	log::set(&std::cerr);

	MatchBuilder match;
    match.add(OFB_IN_PORT{13});

    InstructionSet instructions;
    instructions.add(IT_GOTO_TABLE{3});

    FlowModBuilder flowMod;
    flowMod.setMatch(match);
    flowMod.setInstructions(instructions);

	ByteList buf;
	serialize(flowMod, &buf);
	log::debug("size:", buf.size());
	log::debug(RawDataToHex(buf.data(), buf.size()));

	const FlowMod *msg = reinterpret_cast<const FlowMod *>(buf.data());
	ByteList output;
	yaml::write(msg, &output);
}
