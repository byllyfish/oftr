#include "ofp/unittest.h"
#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/yconstants.h"

using namespace ofp;

// Helper function to test that scalar string output can be parsed back to original value.

template <class T>
static void test_roundtrip(const T &value, void *ctxt) {
    std::string buf;
    llvm::raw_string_ostream out{buf};
    llvm::yaml::ScalarTraits<T>::output(value, ctxt, out);

    T result;
    auto err = llvm::yaml::ScalarTraits<T>::input(out.str(), ctxt, result);
    ASSERT_TRUE(err.empty());
    EXPECT_EQ(result, value);
}

TEST(yconstants, test_message_type) {
	// Test all possible values for OFPType.
	for (unsigned type = 0; type <= 0xFF; ++type) {
		MessageType msgType{static_cast<OFPType>(type)};
		test_roundtrip(msgType, nullptr);
	}

	// Test all possible values for Multipart Request and Reply.
	OFPType types[] = { OFPT_MULTIPART_REQUEST, OFPT_MULTIPART_REPLY };
	for (auto type: types) {
		// Test every single byte multipart type.
		for (unsigned subtype = 0; subtype <= 0xFF; ++subtype) {
			MessageType msgType{type, static_cast<OFPMultipartType>(subtype)};
			test_roundtrip(msgType, nullptr);
		}

		// Test experimenter multipart request/reply.
		MessageType expType{type, OFPMP_EXPERIMENTER};
		test_roundtrip(expType, nullptr);
	}
}

TEST(yconstants, test_multipart_unknown_request) {
	MessageType value{OFPT_MULTIPART_REQUEST, static_cast<OFPMultipartType>(0xABC)};

    std::string buf;
    llvm::raw_string_ostream out{buf};
    llvm::yaml::ScalarTraits<MessageType>::output(value, nullptr, out);

    EXPECT_EQ(out.str(), "REQUEST.0x0ABC");
}

TEST(yconstants, test_multipart_unknown_reply) {
	MessageType value{OFPT_MULTIPART_REPLY, static_cast<OFPMultipartType>(0xDEF)};

    std::string buf;
    llvm::raw_string_ostream out{buf};
    llvm::yaml::ScalarTraits<MessageType>::output(value, nullptr, out);

    EXPECT_EQ(out.str(), "REPLY.0x0DEF");
}
