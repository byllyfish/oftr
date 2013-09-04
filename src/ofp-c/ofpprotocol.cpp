#include "ofp-c/ofpprotocol.h"
#include "ofp/yaml.h"
#include "ofp/yaml/encoder.h"
#include <assert.h>

using namespace ofp;

/**
 * Converts an OpenFlow protocol YAML message to its binary wire format.
 * @param  inMessage input string in YAML format.
 * @param  outBinary output string containing binary wire representation.
 * @param  outError  output string containing an error message; "" if no error.
 * @return           1 if successful, 0 on error.
 */
 
int ofpProtocolEncode(OFPString *inMessage, OFPString *outBinary,
                      OFPString *outError)
{
	using yaml::Encoder;

	assert(inMessage);
	assert(outBinary);
	assert(outError);
	
	if (inMessage->length == 0) {
		const char *errMsg = "Empty message.";
		ofpStringSet(outError, errMsg, strlen(errMsg));
		ofpStringClear(outBinary);
		return 0;
	}

	Encoder encoder;
	llvm::StringRef input{inMessage->data, Unsigned_cast(inMessage->length)};
	llvm::yaml::Input yin(input);
	yin.setDiagHandler(Encoder::diagnosticHandler, &encoder);
	yin >> encoder;

	if (yin.error()) {
		const std::string &err = encoder.error();
		ofpStringClear(outBinary);
		ofpStringSet(outError, err.data(), err.length());
		return 0;
	}

	ofpStringSet(outBinary, encoder.data(), encoder.size());
	ofpStringClear(outError);

	return 1;
}

/**
 * Converts an OpenFlow protocol message from its binary wire format to YAML.
 * @param  inBinary   input string in binary wire representation.
 * @param  outMessage output string containing YAML representation.
 * @param  outError   output string containing an error message; "" if no error.
 * @return            1 if successful, 0 on error.
 */
int ofpProtocolDecode(OFPString *inBinary, OFPString *outMessage,
                      OFPString *outError)
{
	return 0;
}
