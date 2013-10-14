# Build nullagent and nullcontroller.

add_executable(nullagent "nullagent_main.cpp")
target_link_libraries(nullagent ofp ${CMAKE_THREAD_LIBS_INIT})

add_executable(nullcontroller "nullcontroller_main.cpp")
target_link_libraries(nullcontroller ofp ${CMAKE_THREAD_LIBS_INIT})

add_test(
  NAME nulldemo
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests.sh ${CMAKE_CURRENT_BINARY_DIR}
)

set_tests_properties(nulldemo PROPERTIES TIMEOUT 10)
