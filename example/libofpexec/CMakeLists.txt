
add_executable(libofpexec libofpexec_main.cpp)
target_link_libraries(libofpexec ofp ${CMAKE_THREAD_LIBS_INIT})
install(TARGETS libofpexec DESTINATION bin)

add_subdirectory(test)
