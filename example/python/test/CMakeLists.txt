add_test(
  NAME python_json
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/run.sh $<TARGET_FILE:testagent> $<TARGET_FILE:libofpexec> 0
)

set_tests_properties(python_json PROPERTIES TIMEOUT 10)

add_test(
  NAME python_yaml
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/run.sh $<TARGET_FILE:testagent> $<TARGET_FILE:libofpexec> 1
)

set_tests_properties(python_yaml PROPERTIES TIMEOUT 10)
