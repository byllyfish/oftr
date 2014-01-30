# Build testagent.

add_executable(testagent testagent_main.cpp testagent.cpp)
target_link_libraries(testagent ofp ${LIBOFP_LINKED_LIBS})

add_subdirectory(test)
