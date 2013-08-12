#ifndef OFP_FEATURES_H
#define OFP_FEATURES_H

#include "ofp/padding.h"
#include "ofp/datapathid.h"

namespace ofp { // <namespace ofp>

/**
 *  Features is a concrete class representing the features of a switch. {7.3.1}
 *
 *  Uses native byte order.
 */
class Features {
public:
	Features() = default;

	DatapathID datapathID() const {
		return datapathID_;
	}

	UInt32 bufferCount() const {
		return bufferCount_;
	}

	UInt8  tableCount() const {
		return tableCount_;
	}

	UInt8	auxiliaryID() const {
		return auxiliaryID_;
	}

	UInt32  capabilities() const {
		return capabilities_;
	}

	UInt32 reserved() const {
		return reserved_;
	}


	void setDatapathID(DatapathID datapathID) {
		datapathID_ = datapathID;
	}

	void setBufferCount(UInt32 bufferCount) {
		bufferCount_ = bufferCount;
	}

	void  setTableCount(UInt8 tableCount) {
		tableCount_ = tableCount;
	}

	void setAuxiliaryID(UInt8 auxiliaryID) {
		auxiliaryID_ = auxiliaryID;
	}

	void setCapabilities(UInt32 capabilities) {
		capabilities_ = capabilities;
	}

	void setReserved(UInt32 reserved) {
		reserved_ = reserved;
	}

private:
	DatapathID datapathID_;
	UInt32 bufferCount_;
	UInt8  tableCount_;
	UInt8	auxiliaryID_;
	Padding<2> pad_;
	UInt32  capabilities_;
	UInt32 reserved_;
};

} // </namespace ofp>

#endif // OFP_FEATURES_H
