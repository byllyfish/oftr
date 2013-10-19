# This file needs to be in the cmake build tree. It is copied into the build 
# tree by 'configure_file'.

# Specify tests to ignore during memcheck phase.

set(CTEST_CUSTOM_MEMCHECK_IGNORE 
	nulldemo
	testagent
	controller
	libofpexec
	python
)
