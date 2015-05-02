add_test(
  NAME libofp-test
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests.sh
)

set_tests_properties(libofp-test PROPERTIES TIMEOUT 360)


add_test(
  NAME libofp-annotate
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-annotate.sh
)

set_tests_properties(libofp-annotate PROPERTIES TIMEOUT 600)
