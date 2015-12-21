add_test(NAME controller COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/run.sh $<TARGET_FILE:testagent>)
set_tests_properties(controller PROPERTIES TIMEOUT 10 ENVIRONMENT "LIBOFP_DEFAULT_PORT=16653")

