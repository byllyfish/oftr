add_test(NAME nulldemo COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/run.sh)
set_tests_properties(nulldemo PROPERTIES TIMEOUT 10 ENVIRONMENT "LIBOFP_DEFAULT_PORT=8661")
