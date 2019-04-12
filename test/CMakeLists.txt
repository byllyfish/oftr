
set(LIBOFP_TEST_SOURCES
	ofp/actionlist_unittest.cpp
	ofp/actions_unittest.cpp
	ofp/actionrange_unittest.cpp
	ofp/actiontype_unittest.cpp
	ofp/bucketlist_unittest.cpp
	ofp/bundleaddmessage_unittest.cpp
	ofp/bundlecontrol_unittest.cpp
	ofp/bytelist_unittest.cpp
	ofp/byteorder_unittest.cpp
	ofp/byterange_unittest.cpp
	ofp/channeloptions_unittest.cpp
	ofp/constants_unittest.cpp
	ofp/constants_error_unittest.cpp
	ofp/datapathid_unittest.cpp
	ofp/durationsec_unittest.cpp
	ofp/decoder_unittest.cpp
	ofp/echo_unittest.cpp
	ofp/encoder_unittest.cpp
	ofp/encoderfail_unittest.cpp
	ofp/macaddress_unittest.cpp
	ofp/enumconverter_unittest.cpp
	ofp/error_unittest.cpp
	ofp/featuresreply_unittest.cpp
	ofp/flowmodbuilder_unittest.cpp
	ofp/flowremovedbuilder_unittest.cpp
	ofp/flowremovedv6_unittest.cpp
	ofp/hash_unittest.cpp
	ofp/instructionrange_unittest.cpp
	ofp/libofp_unittest.cpp
	ofp/lldpvalue_unittest.cpp
	ofp/log_unittest.cpp
	ofp/macaddress_unittest.cpp
	ofp/meterbandlist_unittest.cpp
	ofp/mpflowmonitorrequest_unittest.cpp
	ofp/mpflowmonitorreply_unittest.cpp
	ofp/mpflowstatsreply_unittest.cpp
	ofp/mpportstats_unittest.cpp
	ofp/header_unittest.cpp
	ofp/headeronly_unittest.cpp
	ofp/hello_unittest.cpp
	ofp/instructions_unittest.cpp
	ofp/instructionset_unittest.cpp
	ofp/ipv4address_unittest.cpp
	ofp/ipv6address_unittest.cpp
	ofp/ipv6endpoint_unittest.cpp
	ofp/getjson_unittest.cpp
	ofp/matchbuilder_unittest.cpp
	ofp/matchheader_unittest.cpp
	ofp/matchpacket_unittest.cpp
	ofp/matchpacketbuilder_unittest.cpp
	ofp/matchpacket_fuzz_unittest.cpp
	ofp/message_unittest.cpp
	ofp/multipartreply_unittest.cpp
	ofp/nicira_unittest.cpp
	ofp/originalmatch_unittest.cpp
	ofp/outputjson_unittest.cpp
	ofp/oxmfields_unittest.cpp
	ofp/oxmfulltype_unittest.cpp
	ofp/oxmiterator_unittest.cpp
	ofp/oxmlist_unittest.cpp
	ofp/oxmrange_unittest.cpp
	ofp/oxmtype_unittest.cpp
	ofp/oxmregister_unittest.cpp
	ofp/oxmtypeset_unittest.cpp
	ofp/oxmvalue_unittest.cpp
	ofp/packetin_unittest.cpp
	ofp/packetout_unittest.cpp
	ofp/packetoutv6_unittest.cpp
	ofp/padding_unittest.cpp
	ofp/pkt_unittest.cpp
	ofp/port_unittest.cpp
	ofp/portstatus_unittest.cpp
	ofp/protocoliterator_unittest.cpp
	ofp/protocolrange_unittest.cpp
	ofp/protocolversions_unittest.cpp
	ofp/queue_unittest.cpp
	ofp/queuedesc_unittest.cpp
	ofp/queuegetconfigreply_unittest.cpp
	ofp/queuegetconfigrequest_unittest.cpp
	ofp/requestforward_unittest.cpp
	ofp/rolereply_unittest.cpp
	ofp/rolestatus_unittest.cpp
	ofp/setasync_unittest.cpp
	ofp/smallbuffer_unittest.cpp
	ofp/smallcstring_unittest.cpp
	ofp/standardmatch_unittest.cpp
	ofp/tabledesc_unittest.cpp
	ofp/tablemod_unittest.cpp
	ofp/tablestatus_unittest.cpp
	ofp/timestamp_unittest.cpp
	ofp/types_unittest.cpp
	ofp/unittest_unittest.cpp
	ofp/validateinput_unittest.cpp
	ofp/vlannumber_unittest.cpp
	ofp/ybyteorder_unittest.cpp
	ofp/yconstants_unittest.cpp
	ofp/yhello_unittest.cpp
	ofp/ymatch_unittest.cpp
	ofp/ysmallcstring_unittest.cpp
	ofp/demux/flowcache_unittest.cpp
	ofp/demux/flowstate_unittest.cpp
	ofp/demux/segmentcache_unittest.cpp
	"${CMAKE_SOURCE_DIR}/src/ofp/libofp.cpp"
)

if(LIBOFP_ENABLE_JSONRPC)
	set(LIBOFP_TEST_SOURCES
		${LIBOFP_TEST_SOURCES}
		ofp/asio_unittest.cpp
		ofp/boost_asio_unittest.cpp
		ofp/driver_unittest.cpp
		ofp/roundtrip_unittest.cpp
		ofp/rpcencoder_unittest.cpp
		ofp/rpcevents_unittest.cpp
		ofp/rpc/filteractiongenericreply_unittest.cpp
		ofp/rpc/filtertable_unittest.cpp
		ofp/rpc/ratelimiter_unittest.cpp
	)
	if(LIBOFP_ENABLE_OPENSSL)
		set(LIBOFP_TEST_SOURCES
			${LIBOFP_TEST_SOURCES}
        	ofp/identity_unittest.cpp
        )
	endif()
endif()

if(HAVE_LIBPCAP)
    set(LIBOFP_TEST_SOURCES
    	${LIBOFP_TEST_SOURCES}
    	ofp/demux/pktsink_unittest.cpp
    	ofp/demux/pktfilter_unittest.cpp
    )
endif()

# Include 'gtest-all.cc' source file when building ofp_unittest. Avoid googletest's
# CMakeLists.txt because it wants to install googletest.

set(gtest_SOURCE_DIR "${CMAKE_SOURCE_DIR}/external/googletest/googletest")
include_directories(SYSTEM "${gtest_SOURCE_DIR}/include" "${gtest_SOURCE_DIR}")

# -Wzero-as-null-pointer-constant poses a problem for EXPECT_EQ(0, blah) so turn it off
# until googletest fixes this.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-zero-as-null-pointer-constant")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-zero-as-null-pointer-constant")

add_executable(ofp_unittest ofp/ofp_unittest_main.cpp ${LIBOFP_TEST_SOURCES} ${gtest_SOURCE_DIR}/src/gtest-all.cc)
target_link_libraries(ofp_unittest ofp ${LIBOFP_LINKED_LIBS} ${LIBOFP_LINKED_LIBPCAP})

# Add tests here.

add_test(NAME unittests COMMAND ofp_unittest --gtest_shuffle)
set_tests_properties(unittests PROPERTIES TIMEOUT 60 ENVIRONMENT "LIBOFP_DEFAULT_PORT=8671")

if(LIBOFP_MEMCHECK)
	separate_arguments(cmd UNIX_COMMAND "${LIBOFP_MEMCHECK} --leak-check=full --suppressions=${CMAKE_CURRENT_SOURCE_DIR}/ofp_unittest.supp ./ofp_unittest")
    add_test(NAME memcheck_unittests COMMAND ${cmd})
    set_tests_properties(memcheck_unittests PROPERTIES TIMEOUT 60 ENVIRONMENT "LIBOFP_DEFAULT_PORT=8672;LIBOFP_VALGRIND=1")
endif()
