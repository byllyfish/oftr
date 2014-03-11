add_executable(flowbench flowbench_main.cpp)
target_link_libraries(flowbench ofp ${LIBOFP_LINKED_LIBS})
