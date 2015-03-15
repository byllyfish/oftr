add_test(
  NAME python_json
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/run.sh $<TARGET_FILE:testagent> $<TARGET_FILE:ofpx>
)

set_tests_properties(python_json PROPERTIES TIMEOUT 10)
