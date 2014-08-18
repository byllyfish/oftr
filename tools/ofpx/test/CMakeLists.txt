add_test(
  NAME ofpx
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests.sh
)

set_tests_properties(ofpx PROPERTIES TIMEOUT 180)


add_test(
  NAME ofpx-annotate
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-annotate.sh
)

set_tests_properties(ofpx-annotate PROPERTIES TIMEOUT 180)
