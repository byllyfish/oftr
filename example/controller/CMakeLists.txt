
add_executable(simple simple_main.cpp simplecontroller.cpp simplechannellistener.cpp)
target_link_libraries(simple ofp ${CMAKE_THREAD_LIBS_INIT})