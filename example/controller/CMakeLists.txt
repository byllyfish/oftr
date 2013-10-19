add_executable(controller controller_main.cpp simplecontroller.cpp simplechannellistener.cpp)
target_link_libraries(controller ofp ${CMAKE_THREAD_LIBS_INIT})

add_subdirectory(test)