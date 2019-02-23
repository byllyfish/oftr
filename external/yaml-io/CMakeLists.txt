

set(YAMLIO_SOURCES
	src/MakeLinkerHappy.cpp
	src/Support/Allocator.cpp
	src/Support/APInt.cpp
	src/Support/Atomic.cpp
	src/Support/CommandLine.cpp
	src/Support/ConvertUTF.cpp
	src/Support/ConvertUTFWrapper.cpp
	src/Support/Errno.cpp
	src/Support/FoldingSet.cpp
	src/Support/Hashing.cpp
    src/Support/LineIterator.cpp
	src/Support/ManagedStatic.cpp
	src/Support/Memory.cpp
	src/Support/MemoryBuffer.cpp
	src/Support/Mutex.cpp
	src/Support/NativeFormatting.cpp
	src/Support/Path.cpp
	src/Support/Process.cpp
	src/Support/Program.cpp
	src/Support/raw_ostream.cpp
	src/Support/SmallPtrSet.cpp
	src/Support/SmallVector.cpp
	src/Support/SourceMgr.cpp
    src/Support/StringExtras.cpp
	src/Support/StringMap.cpp
	src/Support/StringRef.cpp
    src/Support/StringSaver.cpp
	src/Support/Threading.cpp
	src/Support/Twine.cpp
	src/Support/Unicode.cpp
	src/Support/Valgrind.cpp
	src/Support/WithColor.cpp
	src/Support/YAMLParser.cpp
	src/Support/YAMLTraits.cpp
)

#include(config-ix)
include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckCXXSourceCompiles)

# Helper macros and functions (not used)
# macro(add_cxx_include result files)
#   set(${result} "")
#   foreach (file_name ${files})
#      set(${result} "${${result}}#include<${file_name}>\n")
#   endforeach()
# endmacro(add_cxx_include files result)

# function(check_type_exists type files variable)
#   add_cxx_include(includes "${files}")
#   CHECK_CXX_SOURCE_COMPILES("
#     ${includes} ${type} typeVar;
#     int main() {
#         return 0;
#     }
#     " ${variable})
# endfunction()

check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(sys/mman.h HAVE_SYS_MMAN_H)
check_include_file(sys/stat.h HAVE_SYS_STAT_H)
check_include_file(dirent.h HAVE_DIRENT_H)
check_include_file(fcntl.h HAVE_FCNTL_H)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(sys/param.h HAVE_SYS_PARAM_H)
check_include_file(signal.h HAVE_SIGNAL_H)
check_include_file(sys/time.h HAVE_SYS_TIME_H)
check_include_file(sys/resource.h HAVE_SYS_RESOURCE_H)
check_include_file(malloc.h HAVE_MALLOC_H)
check_include_file(malloc/malloc.h HAVE_MALLOC_MALLOC_H)
check_include_file(sys/wait.h HAVE_SYS_WAIT_H)

check_symbol_exists(getrusage sys/resource.h HAVE_GETRUSAGE)
check_symbol_exists(futimens sys/stat.h HAVE_FUTIMENS)
check_symbol_exists(futimes sys/time.h HAVE_FUTIMES)
check_symbol_exists(getpagesize unistd.h HAVE_GETPAGESIZE)
check_symbol_exists(mallinfo malloc.h HAVE_MALLINFO)
check_symbol_exists(malloc_zone_statistics malloc/malloc.h HAVE_MALLOC_ZONE_STATISTICS)
check_symbol_exists(arc4random stdlib.h HAVE_DECL_ARC4RANDOM)

# set(headers "sys/types.h")

# if (HAVE_INTTYPES_H)
#   set(headers ${headers} "inttypes.h")
# endif()

# if (HAVE_STDINT_H)
#   set(headers ${headers} "stdint.h")
# endif()

# check_type_exists(int64_t "${headers}" HAVE_INT64_T)
# check_type_exists(uint64_t "${headers}" HAVE_UINT64_T)
# check_type_exists(u_int64_t "${headers}" HAVE_U_INT64_T)

# C++11 only; just assume they exist.
set(HAVE_INT64_T 1)
set(HAVE_UINT64_T 1)
set(HAVE_U_INT64_T 0)

set(PACKAGE_NAME "yamlio")
set(PACKAGE_VERSION "4.0.0svn")
set(LLVM_DEFAULT_TARGET_TRIPLE "a-b-c")
set(LLVM_HOST_TRIPLE "a-b-c")

if(WIN32)
  set(LLVM_ON_WIN32 1)
else()
  set(LLVM_ON_UNIX 1)
endif()

configure_file(include/llvm/Config/config.h.cmake include/llvm/Config/config.h)
configure_file(include/llvm/Config/llvm-config.h.cmake include/llvm/Config/llvm-config.h)
configure_file(include/llvm/Config/abi-breaking.h.cmake include/llvm/Config/abi-breaking.h)
configure_file(include/llvm/Support/DataTypes.h.cmake include/llvm/Support/DataTypes.h)

add_library(yamlio ${YAMLIO_SOURCES})

add_subdirectory(unittests)
