# Build testagent.

add_executable(testagent testagent_main.cpp testagent.cpp)
target_link_libraries(testagent ofp ${CMAKE_THREAD_LIBS_INIT})

add_test(
  NAME testagent
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests.sh ${CMAKE_CURRENT_BINARY_DIR}
)

set_tests_properties(testagent PROPERTIES TIMEOUT 10)
