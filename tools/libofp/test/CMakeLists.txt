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


add_test(
  NAME libofp-messages
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-messages.sh
)
set_tests_properties(libofp-messages PROPERTIES TIMEOUT 360)


add_test(
  NAME libofp-schema
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-schema.sh
)
set_tests_properties(libofp-schema PROPERTIES TIMEOUT 10)
