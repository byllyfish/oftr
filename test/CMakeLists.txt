
set(LIBOFP_TEST_SOURCES
	ofp/actionlist_unittest.cpp
	ofp/actions_unittest.cpp
	ofp/actiontype_unittest.cpp
	ofp/asio_unittest.cpp
	ofp/boost_asio_unittest.cpp
	ofp/bucketlist_unittest.cpp
	ofp/bytelist_unittest.cpp
	ofp/byteorder_unittest.cpp
	ofp/byterange_unittest.cpp
	ofp/constants_unittest.cpp
	ofp/datapathid_unittest.cpp
	ofp/decoder_unittest.cpp
	ofp/driver_unittest.cpp
	ofp/dtls_utils_unittest.cpp
	ofp/encoder_unittest.cpp
	ofp/enetaddress_unittest.cpp
	ofp/enumconverter_unittest.cpp
	ofp/error_unittest.cpp
	ofp/featuresreply_unittest.cpp
	ofp/flowmodbuilder_unittest.cpp
	ofp/flowremovedbuilder_unittest.cpp
	ofp/meterbandlist_unittest.cpp
	ofp/mpflowstatsreply_unittest.cpp
	ofp/header_unittest.cpp
	ofp/headeronly_unittest.cpp
	ofp/hello_unittest.cpp
	ofp/instructions_unittest.cpp
	ofp/instructionset_unittest.cpp
	ofp/ipv4address_unittest.cpp
	ofp/ipv6address_unittest.cpp
	ofp/ipv6endpoint_unittest.cpp
	ofp/matchbuilder_unittest.cpp
	ofp/matchheader_unittest.cpp
	ofp/message_unittest.cpp
	ofp/multipartreply_unittest.cpp
	ofp/originalmatch_unittest.cpp
	ofp/outputjson_unittest.cpp
	ofp/oxmfields_unittest.cpp
	ofp/oxmiterator_unittest.cpp
	ofp/oxmlist_unittest.cpp
	ofp/oxmrange_unittest.cpp
	ofp/oxmtype_unittest.cpp
	ofp/oxmtypeset_unittest.cpp
	ofp/oxmvalue_unittest.cpp
	ofp/packetin_unittest.cpp
	ofp/packetout_unittest.cpp
	ofp/padding_unittest.cpp
	ofp/port_unittest.cpp
	ofp/portstatus_unittest.cpp
	ofp/protocolrange_unittest.cpp
	ofp/protocolversions_unittest.cpp
	ofp/queue_unittest.cpp
	ofp/queuegetconfigreply_unittest.cpp
	ofp/queuegetconfigrequest_unittest.cpp
	ofp/roundtrip_unittest.cpp
	ofp/rpcsession_unittest.cpp
	ofp/smallbuffer_unittest.cpp
	ofp/smallcstring_unittest.cpp
	ofp/standardmatch_unittest.cpp
	ofp/timestamp_unittest.cpp
	ofp/types_unittest.cpp
	ofp/unittest_unittest.cpp
	ofp/yhello_unittest.cpp
	ofp/ymatch_unittest.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isystem ${gtest_SOURCE_DIR}/include")

add_executable(ofp_unittest ofp/ofp_unittest_main.cpp ${LIBOFP_TEST_SOURCES})
target_link_libraries(ofp_unittest gtest ofp ${LIBOFP_LINKED_LIBS})

# Add tests here.

add_test(
  NAME unittests
  COMMAND ofp_unittest
)

# Unit tests may need more than 10 seconds when running under valgrind.

set_tests_properties(unittests PROPERTIES TIMEOUT 60)
