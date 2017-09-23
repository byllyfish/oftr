add_test(NAME oftr-test COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests.sh)
set_tests_properties(oftr-test PROPERTIES TIMEOUT 600)

if(LIBOFP_MEMCHECK)
  add_test(NAME memcheck_oftr-test COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests.sh)
  set_tests_properties(memcheck_oftr-test PROPERTIES TIMEOUT 600 ENVIRONMENT "LIBOFP_MEMCHECK=${LIBOFP_MEMCHECK}")
endif()

add_test(NAME oftr-annotate COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-annotate.sh)
set_tests_properties(oftr-annotate PROPERTIES TIMEOUT 600)

if(LIBOFP_MEMCHECK)
  add_test(NAME memcheck_oftr-annotate COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-annotate.sh)
  set_tests_properties(memcheck_oftr-annotate PROPERTIES TIMEOUT 600 ENVIRONMENT "LIBOFP_MEMCHECK=${LIBOFP_MEMCHECK}")
endif()

add_test(NAME oftr-messages COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-messages.sh)
set_tests_properties(oftr-messages PROPERTIES TIMEOUT 360)

if(LIBOFP_MEMCHECK)
  add_test(NAME memcheck_oftr-messages COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-messages.sh)
  set_tests_properties(memcheck_oftr-messages PROPERTIES TIMEOUT 600 ENVIRONMENT "LIBOFP_MEMCHECK=${LIBOFP_MEMCHECK}")
endif()

add_test(NAME oftr-schema COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-schema.sh)
set_tests_properties(oftr-schema PROPERTIES TIMEOUT 10)

if(LIBOFP_MEMCHECK)
  add_test(NAME memcheck_oftr-schema COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-schema.sh)
  set_tests_properties(memcheck_oftr-schema PROPERTIES TIMEOUT 600 ENVIRONMENT "LIBOFP_MEMCHECK=${LIBOFP_MEMCHECK}")
endif()

if(HAVE_LIBPCAP)
  add_test(NAME oftr-pcap COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-pcap.sh)
  set_tests_properties(oftr-pcap PROPERTIES TIMEOUT 10)

  if(LIBOFP_MEMCHECK)
    add_test(NAME memcheck_oftr-pcap COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-pcap.sh)
    set_tests_properties(memcheck_oftr-pcap PROPERTIES TIMEOUT 600 ENVIRONMENT "LIBOFP_MEMCHECK=${LIBOFP_MEMCHECK}")
  endif()
endif()

add_test(NAME oftr-cmdline COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-cmdline.sh)
set_tests_properties(oftr-cmdline PROPERTIES TIMEOUT 10)

if(LIBOFP_MEMCHECK)
  add_test(NAME memcheck_oftr-cmdline COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-cmdline.sh)
  set_tests_properties(memcheck_oftr-cmdline PROPERTIES TIMEOUT 600 ENVIRONMENT "LIBOFP_MEMCHECK=${LIBOFP_MEMCHECK}")
endif()

add_test(NAME oftr-benchmark COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/runtests-benchmark.sh)
set_tests_properties(oftr-benchmark PROPERTIES TIMEOUT 600)
