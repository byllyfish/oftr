
if(LIBOFP_FSANITIZE)
  message(STATUS "python-ctypes tests disabled because AddressSanitizer is in use.")
else()
  add_test(NAME python-ctypes2 COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/ctypes_test2.py $<TARGET_FILE:oftr>)
  set_tests_properties(python-ctypes2 PROPERTIES TIMEOUT 10)

  add_test(NAME go-dlopen COMMAND go run ${CMAKE_CURRENT_SOURCE_DIR}/dlopen.go $<TARGET_FILE:oftr>)
  set_tests_properties(go-dlopen PROPERTIES TIMEOUT 10)
endif()
