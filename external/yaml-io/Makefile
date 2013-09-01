# Makefile for yaml-io

LLVM_DIR = ../../../llvm/include
LLVM_BUILD_DIR = ../../../llvm-build/include
LLVM_LIB = $(LLVM_DIR)/../lib

LLVM_SOURCES = YAMLParser.cpp YAMLTraits.cpp \
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


#LLVM_UNIX_SOURCES = Unix/Memory.inc

LLVM_INCS = llvm/ADT/APInt.h llvm/Support/Allocator.h llvm/Support/Memory.h  \
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

LLVM_BUILD_INCS = llvm/Config/llvm-config.h llvm/Support/DataTypes.h llvm/Config/config.h

LLVM_INCLUDES = $(LLVM_SOURCES:.cpp=.h) 

LLVM_SOURCES_PATH = $(addprefix $(LLVM_LIB)/Support/, $(LLVM_SOURCES))
LLVM_SUPPORT_PATH = $(addprefix $(LLVM_DIR)/llvm/Support/, $(LLVM_INCLUDES))

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

LLVM_OBJECTS1 = $(wildcard src/Support/*.cpp) src/MakeLinkerHappy.cpp
LLVM_OBJECTS = $(LLVM_OBJECTS1:.cpp=.o)

CPPFLAGS += -I ../include -I ofp -std=c++11
#CPPFLAGS += -isystem $(LLVM_DIR) -isystem $(LLVM_BUILD_DIR)
CPPFLAGS += -I include -I ofp -std=c++11
WARNINGS = -Wall

# Need -stdlib on Mac OS X.
ifeq ($(shell uname -s),Darwin)
 CPPFLAGS += -stdlib=libc++
endif

# llvm files expect these...
CPPFLAGS += -D __STDC_LIMIT_MACROS -D __STDC_CONSTANT_MACROS 
CXXFLAGS += -g $(WARNINGS)
#-D NDEBUG

libyamlio.a: $(LLVM_OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

#libyamlio.a: $(LLVM_OBJ)
#	$(AR) $(ARFLAGS) $@ $^

.PHONY: clean
clean:
	rm -f $(LLVM_OBJECTS)
	rm -f libyamlio.a
	rm -f test/testit
	rm -f test/testit.o

.PHONY: dist
dist:
	mkdir -p src/Support/Unix
	mkdir -p include/llvm/ADT include/llvm/Config include/llvm-c
	mkdir -p include/llvm/Support
	for f in $(LLVM_SOURCES); do 					 \
	  cp $(LLVM_LIB)/Support/$$f src/Support/$$f ;   \
	done
	for f in $(LLVM_INCS); do   				\
	  cp $(LLVM_DIR)/$$f include/$$f ;				\
	done
	for f in $(LLVM_BUILD_INCS); do \
	  cp $(LLVM_BUILD_DIR)/$$f include/$$f ; \
	done
	patch include/llvm/Config/config.h src/Config.diff
	patch src/Support/SourceMgr.cpp src/SourceMgr.diff

.PHONY: test
test: test/testit
	./test/testit

test/testit: test/testit.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ -L . -lyamlio

