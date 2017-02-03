
add_test(NAME python-ctypes COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/ctypes_test.py $<TARGET_FILE:oftr>)
set_tests_properties(python-ctypes PROPERTIES TIMEOUT 10)
