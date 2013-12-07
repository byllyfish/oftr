# Build nullagent and nullcontroller.

add_executable(nullagent nullagent_main.cpp)
target_link_libraries(nullagent ofp ${LIBOFP_LINKED_LIBS})

add_executable(nullcontroller nullcontroller_main.cpp)
target_link_libraries(nullcontroller ofp ${LIBOFP_LINKED_LIBS})

add_subdirectory(test)
