#ifndef OFP_INSTRUCTIONS_H
#define OFP_INSTRUCTIONS_H

#include "ofp/instructiontype.h"
#include "ofp/padding.h"
#include "ofp/actionlist.h"

namespace ofp { // <namespace ofp>


class IT_GOTO_TABLE {
public:

	constexpr explicit IT_GOTO_TABLE(UInt8 table) : table_{table} {}

	constexpr size_t size() const { return sizeof(*this); }
	constexpr UInt8	table() const { return table_; }

private:
	const InstructionType type_{InstructionType::IT_GOTO_TABLE};
	const Big16 length_{8};
	const Big8 table_;
	const Padding<3> pad_;
};


class IT_WRITE_METADATA {
public:
	constexpr explicit IT_WRITE_METADATA(UInt64 metadata, UInt64 mask) : metadata_{metadata}, mask_{mask} {}

	constexpr UInt64 metadata() const { return metadata_; }
	constexpr UInt64 mask() const { return mask_; }

private:
	const InstructionType type_{InstructionType::IT_WRITE_METADATA};
	const Big16 length_{24};
	const Padding<4> pad_;
	const Big64	metadata_;
	const Big64	mask_;
};


namespace detail { // <namespace detail>

// IT_WithActions is an instruction with a variable sized action list.

template<UInt16 InstrType>
class IT_WithActions {
public:
	enum { VariableSize = true };
	enum { HeaderSize = 8 };

	explicit IT_WithActions(const ActionList *actions) : length_{HeaderSize + actions->size()}, actions_{actions} {}

	const UInt8 *data() const { return actions_->data(); }
	size_t size() const { return actions_->size(); }

private:
	const InstructionType type_{InstrType};
	const Big16 length_;
	const Padding<4> pad_;
	const ActionList *actions_;
};

} // </namespace detail>


using IT_WRITE_ACTIONS = detail::IT_WithActions<InstructionType::IT_WRITE_ACTIONS>;
using IT_APPLY_ACTIONS = detail::IT_WithActions<InstructionType::IT_APPLY_ACTIONS>;


class IT_CLEAR_ACTIONS {
public:
	constexpr IT_CLEAR_ACTIONS() = default;

private:
	const InstructionType type_{InstructionType::IT_CLEAR_ACTIONS};
	const Big16 length_{8};
	const Padding<4> pad_;
};


class IT_METER {
public:
	constexpr explicit IT_METER(UInt32 meter) : meter_{meter} {}

	constexpr UInt32 meter() const { return meter_; }

private:
	const InstructionType type_{InstructionType::IT_METER};
	const Big16 length_{8};
	const Big32 meter_;
};


class IT_EXPERIMENTER {
public:
	constexpr explicit IT_EXPERIMENTER(UInt32 experimenterid) : experimenterid_{experimenterid} {}

	constexpr UInt32 experimenterid() const { return experimenterid_; }
	
private:
	const InstructionType type_{InstructionType::IT_EXPERIMENTER};
	const Big16 length_{8};
	const Big32 experimenterid_;
};


} // </namespace ofp>

#endif // OFP_INSTRUCTIONS_H
