set(YAMLIO_TEST_SOURCES
    Support/YAMLIOTest.cpp
    Support/YAMLParserTest.cpp
    yamlio_unittest.cpp
)

set(gtest_SOURCE_DIR "${CMAKE_SOURCE_DIR}/external/googletest/googletest")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isystem ${gtest_SOURCE_DIR}/include -isystem ${gtest_SOURCE_DIR}")

add_executable(yamlio_unittest yamlio_unittest_main.cpp ${YAMLIO_TEST_SOURCES} ${gtest_SOURCE_DIR}/src/gtest-all.cc)
target_link_libraries(yamlio_unittest yamlio ${CMAKE_THREAD_LIBS_INIT})

add_test(NAME yaml_test COMMAND yamlio_unittest)
set_tests_properties(yaml_test PROPERTIES TIMEOUT 10)

if(LIBOFP_MEMCHECK)
  separate_arguments(cmd UNIX_COMMAND "${LIBOFP_MEMCHECK} ./yamlio_unittest")
  add_test(NAME memcheck_yaml_test COMMAND ${cmd})
  set_tests_properties(memcheck_yaml_test PROPERTIES TIMEOUT 60)
endif()
