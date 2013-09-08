# Makefile for yaml-io

# Path to `llvm` and `llvm-build` directories.
LLVM_DIST_PATH = ../../../llvm
LLVM_DIST_BUILD_PATH = ../../../llvm-build

LLVM_INCLUDE_PATH = $(LLVM_DIST_PATH)/include
LLVM_LIB_PATH = $(LLVM_DIST_PATH)/lib
LLVM_BUILD_INCLUDE_PATH = $(LLVM_DIST_BUILD_PATH)/include

LLVM_DIST_SRCS = YAMLParser.cpp YAMLTraits.cpp \
   raw_ostream.cpp SmallVector.cpp \
   StringRef.cpp APInt.cpp  MemoryBuffer.cpp \
   Allocator.cpp system_error.cpp \
   SourceMgr.cpp Twine.cpp \
   Hashing.cpp FoldingSet.cpp \
   Process.cpp Path.cpp \
   TimeValue.cpp Program.cpp \
   Memory.cpp Valgrind.cpp \
   StringMap.cpp Mutex.cpp \
   Errno.cpp Unix/Memory.inc Unix/Unix.h Unix/Path.inc Unix/Process.inc \
   Unix/Program.inc Unix/TimeValue.inc Unix/system_error.inc

LLVM_DIST_INCS = llvm/ADT/APInt.h llvm/Support/Allocator.h llvm/Support/Memory.h  \
    llvm/Support/Path.h llvm/Support/Program.h llvm/Support/TimeValue.h \
    llvm/Support/YAMLParser.h llvm/Support/raw_ostream.h llvm/Support/Errno.h \
    llvm/Support/MemoryBuffer.h llvm/Support/Process.h  \
    llvm/Support/SourceMgr.h llvm/Support/Valgrind.h \
    llvm/Support/YAMLTraits.h  llvm/Support/system_error.h \
    llvm/ADT/ArrayRef.h llvm/ADT/None.h llvm/ADT/SmallVector.h llvm/Support/AlignOf.h \
    llvm/Support/Compiler.h llvm/Support/MathExtras.h llvm/Support/SwapByteOrder.h \
    llvm/Support/type_traits.h llvm/ADT/FoldingSet.h llvm/ADT/StringRef.h \
    llvm/ADT/Hashing.h llvm/ADT/STLExtras.h llvm/Support/Host.h \
    llvm/ADT/StringMap.h llvm/ADT/SmallString.h llvm/Support/Debug.h \
    llvm/Support/ErrorHandling.h llvm/Support/FileSystem.h llvm/ADT/IntrusiveRefCntPtr.h \
    llvm/Support/Casting.h llvm/ADT/OwningPtr.h llvm/ADT/Twine.h llvm/Support/Recycler.h \
    llvm/ADT/ilist.h llvm/Support/CBindingWrapping.h llvm/Support/Endian.h \
    llvm/Support/SMLoc.h llvm/Support/Locale.h llvm-c/Core.h \
    llvm/ADT/edit_distance.h llvm/ADT/StringExtras.h llvm/ADT/ilist_node.h \
    llvm/ADT/DenseMap.h llvm/ADT/DenseMapInfo.h \
    llvm/Support/PointerLikeTypeTraits.h llvm/ADT/StringSwitch.h \
    llvm/Support/Format.h llvm/Support/Mutex.h llvm/Support/Threading.h llvm/Support/MutexGuard.h

LLVM_DIST_BUILD_INCS = llvm/Config/llvm-config.h llvm/Support/DataTypes.h llvm/Config/config.h

LLVM_SOURCES = $(wildcard src/Support/*.cpp) src/MakeLinkerHappy.cpp
LLVM_OBJECTS = $(LLVM_SOURCES:.cpp=.o)

WARNINGS = -Wall

CPPFLAGS += -I include -std=c++11 
#CPPFLAGS += -D NDEBUG
# llvm files expect these macros.
CPPFLAGS += -D __STDC_LIMIT_MACROS -D __STDC_CONSTANT_MACROS 
# Need -stdlib on Mac OS X.
ifeq ($(shell uname -s),Darwin)
 CPPFLAGS += -stdlib=libc++
endif

CXXFLAGS += -g $(WARNINGS)

all: libyamlio.a test

libyamlio.a: $(LLVM_OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

.PHONY: clean
clean:
	rm -f $(LLVM_OBJECTS)
	rm -f libyamlio.a
	rm -f test/testit
	rm -f test/testit.o

.PHONY: test
test: test/testit
	./test/testit

test/testit: test/testit.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ -L . -lyamlio

# Use `make dist` to produce a distribution from existing LLVM sources.
.PHONY: dist
dist:
	mkdir -p src/Support/Unix
	mkdir -p include/llvm/ADT include/llvm/Config include/llvm-c
	mkdir -p include/llvm/Support
	for f in $(LLVM_DIST_SRCS); do 					 	  \
	  cp $(LLVM_LIB_PATH)/Support/$$f src/Support/$$f ;   \
	done
	for f in $(LLVM_DIST_INCS); do   					  \
	  cp $(LLVM_INCLUDE_PATH)/$$f include/$$f ;			  \
	done
	for f in $(LLVM_DIST_BUILD_INCS); do 				  \
	  cp $(LLVM_BUILD_INCLUDE_PATH)/$$f include/$$f ; 	  \
	done
	patch include/llvm/Config/config.h src/Config.diff
	patch src/Support/SourceMgr.cpp src/SourceMgr.diff
