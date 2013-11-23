

set(YAMLIO_SOURCES
	src/MakeLinkerHappy.cpp
	src/Support/Allocator.cpp
	src/Support/APInt.cpp
	src/Support/Atomic.cpp
	src/Support/CommandLine.cpp
	src/Support/ConvertUTF.c
	src/Support/ConvertUTFWrapper.cpp
	src/Support/Errno.cpp
	src/Support/FoldingSet.cpp
	src/Support/Hashing.cpp
	src/Support/Host.cpp
	src/Support/ManagedStatic.cpp
	src/Support/Memory.cpp
	src/Support/MemoryBuffer.cpp
	src/Support/Mutex.cpp
	src/Support/Path.cpp
	src/Support/Process.cpp
	src/Support/Program.cpp
	src/Support/raw_ostream.cpp
	src/Support/SmallPtrSet.cpp
	src/Support/SmallVector.cpp
	src/Support/SourceMgr.cpp
	src/Support/StringMap.cpp
	src/Support/StringRef.cpp
	src/Support/system_error.cpp
	src/Support/Threading.cpp
	src/Support/TimeValue.cpp
	src/Support/Triple.cpp
	src/Support/Twine.cpp
	src/Support/Valgrind.cpp
	src/Support/YAMLParser.cpp
	src/Support/YAMLTraits.cpp
)

add_definitions(-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS)
add_library(yamlio ${YAMLIO_SOURCES})

add_executable(yamlio_test test/testit.cpp)
target_link_libraries(yamlio_test yamlio)

add_test(yaml_test yamlio_test)
set_tests_properties(yaml_test PROPERTIES TIMEOUT 10)

