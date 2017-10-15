# Build cbenchreply controller.

add_executable(cbenchreply cbenchreply_main.cpp)
target_link_libraries(cbenchreply ofp ${LIBOFP_LINKED_LIBS})
