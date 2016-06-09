add_test(NAME libofp-test COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests.sh)
set_tests_properties(libofp-test PROPERTIES TIMEOUT 360)

if(LIBOFP_MEMCHECK)
  add_test(NAME memcheck_libofp-test COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests.sh)
  set_tests_properties(memcheck_libofp-test PROPERTIES TIMEOUT 600 ENVIRONMENT "LIBOFP_MEMCHECK=${LIBOFP_MEMCHECK}")
endif()

add_test(NAME libofp-annotate COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-annotate.sh)
set_tests_properties(libofp-annotate PROPERTIES TIMEOUT 600)

if(LIBOFP_MEMCHECK)
  add_test(NAME memcheck_libofp-annotate COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-annotate.sh)
  set_tests_properties(memcheck_libofp-annotate PROPERTIES TIMEOUT 600 ENVIRONMENT "LIBOFP_MEMCHECK=${LIBOFP_MEMCHECK}")
endif()

add_test(NAME libofp-messages COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-messages.sh)
set_tests_properties(libofp-messages PROPERTIES TIMEOUT 360)

if(LIBOFP_MEMCHECK)
  add_test(NAME memcheck_libofp-messages COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-messages.sh)
  set_tests_properties(memcheck_libofp-messages PROPERTIES TIMEOUT 600 ENVIRONMENT "LIBOFP_MEMCHECK=${LIBOFP_MEMCHECK}")
endif()

add_test(NAME libofp-schema COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-schema.sh)
set_tests_properties(libofp-schema PROPERTIES TIMEOUT 10)

if(LIBOFP_MEMCHECK)
  add_test(NAME memcheck_libofp-schema COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-schema.sh)
  set_tests_properties(memcheck_libofp-schema PROPERTIES TIMEOUT 600 ENVIRONMENT "LIBOFP_MEMCHECK=${LIBOFP_MEMCHECK}")
endif()

add_test(NAME libofp-pcap COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-pcap.sh)
set_tests_properties(libofp-pcap PROPERTIES TIMEOUT 10)

if(LIBOFP_MEMCHECK)
  add_test(NAME memcheck_libofp-pcap COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-pcap.sh)
  set_tests_properties(memcheck_libofp-pcap PROPERTIES TIMEOUT 600 ENVIRONMENT "LIBOFP_MEMCHECK=${LIBOFP_MEMCHECK}")
endif()
