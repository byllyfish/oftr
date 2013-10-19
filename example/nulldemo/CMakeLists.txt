# Build nullagent and nullcontroller.

add_executable(nullagent nullagent_main.cpp)
target_link_libraries(nullagent ofp ${CMAKE_THREAD_LIBS_INIT})

add_executable(nullcontroller nullcontroller_main.cpp)
target_link_libraries(nullcontroller ofp ${CMAKE_THREAD_LIBS_INIT})

add_subdirectory(test)
