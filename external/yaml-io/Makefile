# Makefile for yaml-io

LLVM_DIR = ../../../llvm/include
LLVM_BUILD_DIR = ../../../llvm-build/include
LLVM_LIB = $(LLVM_DIR)/../lib
LLVM_OBJ = $(LLVM_LIB)/Support/YAMLParser.o $(LLVM_LIB)/Support/YAMLTraits.o \
   $(LLVM_LIB)/Support/raw_ostream.o $(LLVM_LIB)/Support/SmallVector.o \
   $(LLVM_LIB)/Support/StringRef.o $(LLVM_LIB)/Support/APInt.o $(LLVM_LIB)/Support/MemoryBuffer.o \
   $(LLVM_LIB)/Support/Allocator.o $(LLVM_LIB)/Support/system_error.o \
   $(LLVM_LIB)/Support/SourceMgr.o $(LLVM_LIB)/Support/Twine.o \
   $(LLVM_LIB)/Support/Hashing.o $(LLVM_LIB)/Support/FoldingSet.o \
   $(LLVM_LIB)/Support/Process.o $(LLVM_LIB)/Support/Path.o \
   $(LLVM_LIB)/Support/TimeValue.o $(LLVM_LIB)/Support/Program.o \
   $(LLVM_LIB)/Support/Memory.o $(LLVM_LIB)/Support/Valgrind.o \
   $(LLVM_LIB)/Support/Errno.o MakeLinkerHappy.o #$(LLVM_LIB)/Support/ErrorHandling.o #$(LLVM_LIB)/Support/Debug.o \
   #$(LLVM_LIB)/Support/CommandLine.o 


CPPFLAGS += -I ../include -I ofp -isystem $(LLVM_DIR) -isystem $(LLVM_BUILD_DIR) -std=c++11

# Need -stdlib on Mac OS X.
ifeq ($(shell uname -s),Darwin)
 CPPFLAGS += -stdlib=libc++
 WARNINGS = -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-documentation
else
 WARNINGS = -Wall
endif

# llvm files expect these...
CPPFLAGS += -D __STDC_LIMIT_MACROS -D __STDC_CONSTANT_MACROS -D NDEBUG
CXXFLAGS += -g $(WARNINGS)


libyamlio.a: $(LLVM_OBJ)
	$(AR) $(ARFLAGS) $@ $^

.PHONY: clean
clean:
	rm -f $(LLVM_OBJ)
	rm -f libyamlio.a


