
add_executable(bridgeofp bridgeofp_main.cpp bridgelistener.cpp)
target_link_libraries(bridgeofp ofp ${CMAKE_THREAD_LIBS_INIT})
