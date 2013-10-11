#ifndef OFP_TABLEMOD_H
#define OFP_TABLEMOD_H

#include "ofp/protocolmsg.h"
#include "ofp/padding.h"

namespace ofp { // <namespace ofp>

class TableMod : public ProtocolMsg<TableMod,OFPT_TABLE_MOD> {
public:
	UInt8 tableId() const { return tableId_; }
	UInt32 config() const { return config_; }

	bool validateLength(size_t length) const;

private:
	Header header_;
	Big8 tableId_;
	Padding<3> pad_;
	Big32 config_;

	// Only TableModBuilder can create an instance.
	TableMod() : header_{type()} {}

	friend class TableModBuilder;
	template <class T>
    friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(TableMod) == 16, "Unexpected size.");
static_assert(IsStandardLayout<TableMod>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<TableMod>(), "Expected trivially copyable.");

class TableModBuilder {
public:
	TableModBuilder() = default;
	explicit TableModBuilder(const TableMod *msg);

	void setTableId(UInt8 tableId) { msg_.tableId_ = tableId; }
	void setConfig(UInt32 config) { msg_.config_ = config; }

	UInt32 send(Writable *channel);

private:
	TableMod msg_;

	template <class T>
    friend struct llvm::yaml::MappingTraits;
};

} // </namespace ofp>

#endif // OFP_TABLEMOD_H
