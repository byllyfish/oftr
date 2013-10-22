#ifndef OFP_QUEUEGETCONFIGREPLY_H
#define OFP_QUEUEGETCONFIGREPLY_H

#include "ofp/protocolmsg.h"

namespace ofp { // <namespace ofp>

// Note: This message is replaced by a MultipartReply in version 1.4.

class QueueGetConfigReply : public ProtocolMsg<QueueGetConfigReply,OFPT_QUEUE_GET_CONFIG_REPLY> {
public:
	UInt32 port() const { return port_; }

private:
	Header header_;
	Big32 port_;
	Padding<4> pad_;

	QueueGetConfigReply() : header_{type()} {}

	friend class QueueGetConfigReplyBuilder;
	template <class T>
    friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(QueueGetConfigReply) == 16, "Unexpected size.");
static_assert(IsStandardLayout<QueueGetConfigReply>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<QueueGetConfigReply>(), "Expected trivially copyable.");

class QueueGetConfigReplyBuilder {
public:
	QueueGetConfigReply() = default;

private:
	QueueGetConfigReply msg_;

	template <class T>
    friend struct llvm::yaml::MappingTraits;
};


} // </namespace ofp>

#endif // OFP_QUEUEGETCONFIGREPLY_H
