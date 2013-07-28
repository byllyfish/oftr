#ifndef OFP_OPENFLOW_TEST_H
#define OFP_OPENFLOW_TEST_H


namespace ofp { // <namespace ofp>
namespace spec { // <namespace spec>

// Define operator == for structs for use in EXPECT_EQ. Due to C++ lookup rules,
// operator == must be defined in ofp::spec.

template <class T>
inline
bool operator==(T a, T b)
{
	return std::memcmp(&a, &b, sizeof(b)) == 0;
}

} // </namespace spec>
} // </namespace ofp>


#endif // OFP_OPENFLOW_TEST_H

