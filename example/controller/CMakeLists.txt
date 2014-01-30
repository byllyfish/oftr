add_executable(controller controller_main.cpp simplecontroller.cpp simplechannellistener.cpp)
target_link_libraries(controller ofp ${LIBOFP_LINKED_LIBS})

add_subdirectory(test)
