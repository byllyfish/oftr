#ifndef OFP_GROUPMOD_H
#define OFP_GROUPMOD_H

#include "ofp/header.h"
#include "ofp/bucketlist.h"
#include "ofp/bucketrange.h"

namespace ofp { // <namespace ofp>

class Message;

class GroupMod {
public:
	static constexpr OFPType type() { return OFPT_GROUP_MOD; }
    static const GroupMod *cast(const Message *message);

    UInt16 command() const { return command_; }
    UInt8 groupType() const { return groupType_; }
    UInt32 groupId() const { return groupId_; }
    BucketRange buckets() const;

    bool validateLength(size_t length) const;

private:
	Header header_;
	Big16 command_;
	Big8 groupType_;
	Padding<1> pad_;
	Big32 groupId_;

	// Only GroupModBuilder can construct an actual instance.
    GroupMod() : header_{type()} {}

    friend class GroupModBuilder;
    template <class T>
    friend struct llvm::yaml::MappingTraits;
};

class GroupModBuilder {
public:
	GroupModBuilder() = default;
	explicit GroupModBuilder(const GroupMod *msg);

	UInt32 send(Writable *channel);

private:
	GroupMod msg_;
	BucketList buckets_;

	template <class T>
    friend struct llvm::yaml::MappingTraits;
};

} // </namespace ofp>

#endif // OFP_GROUPMOD_H
