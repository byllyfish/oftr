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
	WriteToByteList writer{1, 1, data};
	msg.send(&writer);
}


TEST(yaml_flowmod, test) 
{
	FlowModBuilder builder;

	ByteList buf;
	serialize(builder, &buf);
	log::debug("size:", buf.size());

	const FlowMod *msg = reinterpret_cast<const FlowMod *>(buf.data());
	ByteList output;
	yaml::write(msg, &output);
}
