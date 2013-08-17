
DOXYGEN_EXE = /Applications/Doxygen.app/Contents/Resources/doxygen
CLANG_WARNINGS = -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-documentation
WARNINGS = -Wall

IGNORED_SRCS := $(wildcard src/ofp/*_main.cpp)
LIB_SOURCES := $(filter-out $(IGNORED_SRCS),$(wildcard src/ofp/*.cpp) $(wildcard src/ofp/impl/*.cpp))
LIB_OBJECTS := $(patsubst %.cpp,%.o,$(LIB_SOURCES))
LIB_DEPENDS := $(patsubst %.cpp,%.d,$(LIB_SOURCES))

BOOST_ROOT = external/boost_1_54_0_asio
BOOST_OBJECTS = $(BOOST_ROOT)/libs/system/src/error_code.o

CPPFLAGS += -I include -I ofp -isystem $(BOOST_ROOT) -std=c++11
# Need -stdlib on Mac OS X.
ifeq ($(shell uname -s),Darwin)
 CPPFLAGS += -stdlib=libc++
 WARNINGS = $(CLANG_WARNINGS)
endif
CXXFLAGS += -g $(WARNINGS)


libofp: $(LIB_OBJECTS) $(BOOST_OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

.PHONY: docs
docs:
	$(DOXYGEN_EXE) Doxyfile


.PHONY: clean
	rm $(LIB_OBJECTS)
	rm $(BOOST_OBJECTS)
