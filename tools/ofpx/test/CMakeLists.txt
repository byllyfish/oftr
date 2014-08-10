add_test(
  NAME ofpx
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests.sh
)

set_tests_properties(ofpx PROPERTIES TIMEOUT 180)
