

# Points to the root of Google Test, relative to where this file is.
GTEST_DIR = external/googletest

# Flags passed to the preprocessor.
# Set Google Test's header directory as a system directory, such that
# the compiler doesn't generate warnings in Google Test headers.
CPPFLAGS += -isystem $(GTEST_DIR)/include

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -pthread


OBJECTS := $(patsubst %.cpp,%.o,$(wildcard src/*.cpp))
TEST_OBJECTS := $(patsubst %.cpp,%.o,$(wildcard test/*.cpp))

test: $(TEST_OBJECTS)
	$(CC) -o ofp_test $(TEST_OBJECTS)
