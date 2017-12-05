add_executable(flowbench flowbench_main.cpp)
target_link_libraries(flowbench ofp ${LIBOFP_LINKED_LIBS})

if(NOT LIBOFP_ENABLE_CODE_COVERAGE AND NOT CMAKE_BUILD_TYPE MATCHES "Debug")
  add_test(NAME flowbench COMMAND $<TARGET_FILE:flowbench>)
  set_tests_properties(flowbench PROPERTIES TIMEOUT 60)
endif()
