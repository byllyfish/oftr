add_test(
  NAME libofpexec 
  COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests.sh
)

set_tests_properties(libofpexec PROPERTIES TIMEOUT 10)