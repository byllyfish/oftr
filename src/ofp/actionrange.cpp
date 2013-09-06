#include "ofp/actionrange.h"
#include "ofp/actions.h"

namespace ofp { // <namespace ofp>

/// \returns Size of action list when written to channel.
size_t ActionRange::writeSize(Writable *channel)
{
	UInt8 version = channel->version();

	if (version != OFP_VERSION_1) {
		return size();
	}

	size_t newSize = size();

	ActionIterator iter = begin();
	ActionIterator iterEnd = end();

	while (iter != iterEnd) {
		ActionType type = iter.type();

		if (type == AT_OUTPUT::type()) {
			newSize -= 8;
		}

		++iter;
	}

	return newSize;
}

/// \brief Writes action list to channel.
void ActionRange::write(Writable *channel)
{
	UInt8 version = channel->version();

	// Only fix up actions for version 1 (TODO).

	if (version != OFP_VERSION_1) {
		channel->write(data(), size());
		return;
	}

	// Translate AT_OUTPUT's port number from 32 to 16 bits on write.
	
	ActionIterator iter = begin();
	ActionIterator iterEnd = end();

	while (iter != iterEnd) {
		// Write each action separately.
		ActionType type = iter.type();

		if (type == AT_OUTPUT::type()) {
			deprecated::AT_OUTPUT_V1 outAction{iter.action<AT_OUTPUT>()};
			channel->write(&outAction, sizeof(outAction));
		} else {
			channel->write(iter.data(), type.length());
		}

		++iter;
	}
}

} // </namespace ofp>