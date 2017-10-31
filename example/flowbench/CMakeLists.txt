add_executable(flowbench flowbench_main.cpp)
target_link_libraries(flowbench ofp ${LIBOFP_LINKED_LIBS})

add_test(NAME flowbench COMMAND $<TARGET_FILE:flowbench>)
set_tests_properties(flowbench PROPERTIES TIMEOUT 10)
