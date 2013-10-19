add_test(
  NAME testagent
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/run.sh
)

set_tests_properties(testagent PROPERTIES TIMEOUT 10)
