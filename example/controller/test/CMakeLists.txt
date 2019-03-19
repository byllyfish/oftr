add_test(NAME controller COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/run.sh $<TARGET_FILE:testagent>)
set_tests_properties(controller PROPERTIES TIMEOUT 10 ENVIRONMENT "LIBOFP_DEFAULT_PORT=8651")

if(LIBOFP_MEMCHECK)
  add_test(NAME memcheck_controller COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/run.sh $<TARGET_FILE:testagent>)
  set_tests_properties(memcheck_controller PROPERTIES TIMEOUT 30 ENVIRONMENT "LIBOFP_MEMCHECK=${LIBOFP_MEMCHECK};LIBOFP_DEFAULT_PORT=8671")
endif()
