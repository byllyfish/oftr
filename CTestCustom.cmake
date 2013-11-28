# This file needs to be in the cmake build tree. It is copied into the build
# tree by 'configure_file'.

# Specify tests to ignore during memcheck phase.

set(CTEST_CUSTOM_MEMCHECK_IGNORE
	nulldemo
	testagent
	controller
	libofpexec
	python_json
    python_yaml
)

# If PyYaml module is not installed, ignore the python_yaml test.

if(NOT "@PY_YAML_FOUND@" MATCHES "TRUE")
    message("Ignoring python_yaml test.")
    set(CTEST_CUSTOM_TESTS_IGNORE python_yaml)
endif(NOT "@PY_YAML_FOUND@" MATCHES "TRUE")
