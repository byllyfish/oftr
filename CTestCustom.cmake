# This file needs to be in the cmake build tree. It is copied into the build
# tree by 'configure_file'.

# Specify tests to ignore during memcheck phase.

set(CTEST_CUSTOM_MEMCHECK_IGNORE
	nulldemo
	controller
	libofpexec
	python_json
)

# If PyYaml module is not installed, ignore the libofp-annotate test.

if(NOT "@PY_YAML_FOUND@" MATCHES "TRUE")
    message("Ignoring libofp-annotate test.")
    set(CTEST_CUSTOM_TESTS_IGNORE "libofp-annotate")
endif(NOT "@PY_YAML_FOUND@" MATCHES "TRUE")
