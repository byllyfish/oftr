# Build testagent.

add_executable(testagent testagent_main.cpp testagent.cpp)
target_link_libraries(testagent ofp ${CMAKE_THREAD_LIBS_INIT})

add_subdirectory(test)
