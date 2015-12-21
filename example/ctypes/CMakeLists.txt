
add_test(NAME ctypes COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/ctypes_test.py $<TARGET_FILE:libofp>)
set_tests_properties(ctypes PROPERTIES TIMEOUT 10)
