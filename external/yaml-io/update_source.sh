#!/bin/bash
#
# Script to update yaml-io sources from llvm source tree.
#

set -e

function usage()
{
	echo "Usage: $0 <llvm_dir>"
	exit 1
}

LLVM_SOURCE_DIR="$1"
LLVM_BUILD_DIR="${LLVM_SOURCE_DIR}/../llvm-build"
WORKING_DIR=`dirname $0`

test -d "$LLVM_SOURCE_DIR/include" || usage
test -d "$LLVM_SOURCE_DIR/lib/Support" || usage


# Update patch (during development)
#diff -u "${LLVM_SOURCE_DIR}/lib/Support/SourceMgr.cpp" "${WORKING_DIR}/src/Support/SourceMgr.cpp" > "$WORKING_DIR/src/SourceMgr.cpp.diff" || true
#diff -u "${LLVM_SOURCE_DIR}/lib/Support/YAMLTraits.cpp" "${WORKING_DIR}/src/Support/YAMLTraits.cpp" > "$WORKING_DIR/src/YAMLTraits.cpp.diff" || true
#diff -u "${LLVM_SOURCE_DIR}/lib/Support/YAMLParser.cpp" "${WORKING_DIR}/src/Support/YAMLParser.cpp" > "$WORKING_DIR/src/YAMLParser.cpp.diff" || true
#diff -u "${LLVM_SOURCE_DIR}/include/llvm/Support/YAMLTraits.h" "${WORKING_DIR}/include/llvm/Support/YAMLTraits.h" > "$WORKING_DIR/src/YAMLTraits.h.diff" || true
#diff -u "${LLVM_SOURCE_DIR}/lib/Support/Path.cpp" "${WORKING_DIR}/src/Support/Path.cpp" > "$WORKING_DIR/src/Path.cpp.diff" || true
#diff -u "${LLVM_SOURCE_DIR}/lib/Support/Unix/Process.inc" "${WORKING_DIR}/src/Support/Unix/Process.inc" > "$WORKING_DIR/src/Process.inc.diff" || true
#diff -u "${LLVM_SOURCE_DIR}/unittests/Support/YAMLParserTest.cpp" "${WORKING_DIR}/unittests/Support/YAMLParserTest.cpp" > "$WORKING_DIR/src/YAMLParserTest.cpp.diff" || true
#diff -u "${LLVM_SOURCE_DIR}/include/llvm/Support/CommandLine.h" "${WORKING_DIR}/include/llvm/Support/CommandLine.h" > "$WORKING_DIR/src/CommandLine.h.diff" || true

# Include files to copy from llvm source tree.
INCLUDES=(
	include/llvm/ADT/AllocatorList.h
	include/llvm/ADT/APInt.h
	include/llvm/ADT/ArrayRef.h
	include/llvm/ADT/DenseMap.h
	include/llvm/ADT/DenseMapInfo.h
	include/llvm/ADT/edit_distance.h
	include/llvm/ADT/EpochTracker.h
	include/llvm/ADT/FoldingSet.h
	include/llvm/ADT/Hashing.h
	include/llvm/ADT/ilist.h
	include/llvm/ADT/ilist_base.h
	include/llvm/ADT/ilist_iterator.h
	include/llvm/ADT/ilist_node.h
	include/llvm/ADT/ilist_node_base.h
	include/llvm/ADT/ilist_node_options.h
	include/llvm/ADT/IntrusiveRefCntPtr.h
	include/llvm/ADT/iterator.h
	include/llvm/ADT/iterator_range.h
	include/llvm/ADT/None.h
	include/llvm/ADT/Optional.h
	include/llvm/ADT/PointerIntPair.h
	include/llvm/ADT/simple_ilist.h
	include/llvm/ADT/SmallPtrSet.h
	include/llvm/ADT/SmallString.h
	include/llvm/ADT/SmallVector.h
	include/llvm/ADT/STLExtras.h
	include/llvm/ADT/StringExtras.h
	include/llvm/ADT/StringMap.h
	include/llvm/ADT/StringRef.h
	include/llvm/ADT/StringSwitch.h
	include/llvm/ADT/Twine.h
	include/llvm/Config/config.h.cmake
	include/llvm/Config/llvm-config.h.cmake
	include/llvm/Support/AlignOf.h
	include/llvm/Support/Allocator.h
	include/llvm/Support/Atomic.h
	include/llvm/Support/Casting.h
	include/llvm/Support/CBindingWrapping.h
	include/llvm/Support/COFF.h
	include/llvm/Support/CommandLine.h
	include/llvm/Support/Compiler.h
	include/llvm/Support/ConvertUTF.h
	include/llvm/Support/DataStream.h
	include/llvm/Support/DataTypes.h.cmake
	include/llvm/Support/Debug.h
	include/llvm/Support/Endian.h
	include/llvm/Support/Errc.h
	include/llvm/Support/Errno.h
	include/llvm/Support/ErrorHandling.h
	include/llvm/Support/ErrorOr.h
	include/llvm/Support/FileSystem.h
	include/llvm/Support/Format.h
	include/llvm/Support/Host.h
	include/llvm/Support/LineIterator.h
	include/llvm/Support/ManagedStatic.h
	include/llvm/Support/MathExtras.h
	include/llvm/Support/Memory.h
	include/llvm/Support/MemoryBuffer.h
	include/llvm/Support/Mutex.h
	include/llvm/Support/MutexGuard.h
	include/llvm/Support/NativeFormatting.h
	include/llvm/Support/Path.h
	include/llvm/Support/PointerLikeTypeTraits.h
	include/llvm/Support/Process.h
	include/llvm/Support/Program.h
	include/llvm/Support/raw_ostream.h
	include/llvm/Support/Recycler.h
	include/llvm/Support/SMLoc.h
	include/llvm/Support/SourceMgr.h
	include/llvm/Support/StringSaver.h
	include/llvm/Support/SwapByteOrder.h
	include/llvm/Support/Threading.h
	include/llvm/Support/thread.h
	include/llvm/Support/TimeValue.h
	include/llvm/Support/type_traits.h
	include/llvm/Support/Valgrind.h
	include/llvm/Support/WindowsError.h
	include/llvm/Support/YAMLParser.h
	include/llvm/Support/YAMLTraits.h
	include/llvm-c/Core.h
	include/llvm-c/Support.h
	include/llvm-c/Types.h
)

# Source files to copy from llvm source tree.
SOURCES=(
	Support/Allocator.cpp
	Support/APInt.cpp
	Support/Atomic.cpp
	Support/CommandLine.cpp
	Support/ConvertUTF.cpp
	Support/ConvertUTFWrapper.cpp
	Support/Errno.cpp
	Support/FoldingSet.cpp
	Support/Hashing.cpp
	Support/ManagedStatic.cpp
	Support/LineIterator.cpp
	Support/Memory.cpp
	Support/MemoryBuffer.cpp
	Support/Mutex.cpp
	Support/NativeFormatting.cpp
	Support/Path.cpp
	Support/Process.cpp
	Support/Program.cpp
	Support/raw_ostream.cpp
	Support/SmallPtrSet.cpp
	Support/SmallVector.cpp
	Support/SourceMgr.cpp
	Support/StringExtras.cpp
	Support/StringMap.cpp
	Support/StringRef.cpp
	Support/StringSaver.cpp
	Support/Threading.cpp
	Support/TimeValue.cpp
	Support/Twine.cpp
	Support/Unix/Memory.inc
	Support/Unix/Path.inc
	Support/Unix/Process.inc
	Support/Unix/Program.inc
	Support/Unix/TimeValue.inc
	Support/Unix/Unix.h
	Support/Valgrind.cpp
	Support/YAMLParser.cpp
	Support/YAMLTraits.cpp
	Support/Windows/Memory.inc
	Support/Windows/Path.inc
	Support/Windows/Process.inc
	Support/Windows/Program.inc
	Support/Windows/TimeValue.inc
	Support/Windows/WindowsSupport.h
)

TESTS=(
	unittests/Support/YAMLIOTest.cpp
	unittests/Support/YAMLParserTest.cpp
)

# Copy the include files.
for file in "${INCLUDES[@]}"; do
  cp -v "$LLVM_SOURCE_DIR/$file" "$WORKING_DIR/$file"
done

# Copy the source files.
for file in "${SOURCES[@]}"; do
  cp -v "$LLVM_SOURCE_DIR/lib/$file" "$WORKING_DIR/src/$file"
done

# Copy the test files.
for file in "${TESTS[@]}"; do
  cp -v "$LLVM_SOURCE_DIR/$file" "$WORKING_DIR/$file"
done

# Apply patches.

patch "${WORKING_DIR}/src/Support/Path.cpp" "$WORKING_DIR/src/Path.cpp.diff"
patch "${WORKING_DIR}/src/Support/SourceMgr.cpp" "$WORKING_DIR/src/SourceMgr.cpp.diff"
patch "${WORKING_DIR}/src/Support/YAMLTraits.cpp" "$WORKING_DIR/src/YAMLTraits.cpp.diff"
patch "${WORKING_DIR}/src/Support/YAMLParser.cpp" "$WORKING_DIR/src/YAMLParser.cpp.diff"
patch "${WORKING_DIR}/include/llvm/Support/YAMLTraits.h" "$WORKING_DIR/src/YAMLTraits.h.diff"
patch "${WORKING_DIR}/src/Support/Unix/Process.inc" "$WORKING_DIR/src/Process.inc.diff"
patch "${WORKING_DIR}/unittests/Support/YAMLParserTest.cpp" "$WORKING_DIR/src/YAMLParserTest.cpp.diff"
patch "${WORKING_DIR}/include/llvm/Support/CommandLine.h" "$WORKING_DIR/src/CommandLine.h.diff"

exit 0
