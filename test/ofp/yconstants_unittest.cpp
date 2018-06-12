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
		for (unsigned subtype = 0; subtype <= 0xFF; ++subtype) {
			MessageType msgType{type, static_cast<OFPMultipartType>(subtype)};
			test_roundtrip(msgType, nullptr);
		}
	}
}
