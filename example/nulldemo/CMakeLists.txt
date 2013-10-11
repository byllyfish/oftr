# Build nullagent and nullcontroller.

add_executable(nullagent "nullagent_main.cpp")
target_link_libraries(nullagent ofp)

add_executable(nullcontroller "nullcontroller_main.cpp")
target_link_libraries(nullcontroller ofp)

add_test(
  NAME nulldemo
  COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/runtests.sh ${CMAKE_CURRENT_BINARY_DIR}
)