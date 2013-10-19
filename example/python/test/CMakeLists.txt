add_test(
  NAME python
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/run.sh $<TARGET_FILE:testagent> $<TARGET_FILE:libofpexec>
)

set_tests_properties(python PROPERTIES TIMEOUT 10)
