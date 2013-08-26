################################################################################
#  
#  Makefile for libofp
# 
#  Targets to make:
# 
#    libofp.a
#    docs
#    clean
#
################################################################################

# To use `make docs`, specify the full path to your doxygen executable here.

DOXYGEN = /Applications/Doxygen.app/Contents/Resources/doxygen

LIB_SRCS = $(wildcard src/ofp/*.cpp) $(wildcard src/ofp/sys/*.cpp)
LIB_OBJS = $(LIB_SRCS:.cpp=.o)
LIB_DEPS = $(LIB_SRCS:.cpp=.d)

BOOST_ROOT = external/boost_1_54_0_asio
BOOST_OBJECTS = $(BOOST_ROOT)/libs/system/src/error_code.o

export CPPFLAGS += -I include -I ofp -isystem $(BOOST_ROOT) -std=c++11
export CXXFLAGS += -g

# There are some command-line differences between Clang and GCC. GCC doesn't 
# understand the stdlib option. We also want to enable all of Clang's warnings.
 
CXX_VERSION = $(shell $(CXX) --version)
ifeq (clang,$(findstring clang,$(CXX_VERSION)))
 CPPFLAGS += -stdlib=libc++
 ALL_WARNINGS += -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic 
 ALL_WARNINGS += -Wno-unused-parameter -Wno-documentation
else
 ALL_WARNINGS += -Wall
endif
CXXFLAGS += $(ALL_WARNINGS)


libofp.a: $(LIB_OBJS) $(BOOST_OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

.PHONY: docs
docs:
	cd docs; $(DOXYGEN) Doxyfile

.PHONY: clean
clean:
	rm -f libofp.a
	rm -f $(LIB_OBJS)
	rm -f $(BOOST_OBJECTS)


oxmfields: include/ofp/oxmfields.h src/ofp/oxmfields.cpp src/ofp/oxmfieldsid.cpp

./oxm/oxmfields_compile: oxm/oxmfields_compile_main.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

./oxm/oxmfields_compile2: src/ofp/oxmlist.o oxm/oxmfields_main.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

./oxm/oxmfields_compile3: src/ofp/oxmlist.o src/ofp/oxmfields.o oxm/oxmfields_compile3_main.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

./oxm/oxmfields_main.cpp: oxm/oxmfields_compile oxm/oxmfields.tab
	./oxm/oxmfields_compile < oxm/oxmfields.tab > $@

./include/ofp/oxmfields.h: oxm/oxmfields_compile oxm/oxmfields.tab
	./oxm/oxmfields_compile -h < oxm/oxmfields.tab > include/ofp/oxmfields.h

./src/ofp/oxmfields.cpp: oxm/oxmfields_compile2
	./oxm/oxmfields_compile2 > src/ofp/oxmfields.cpp

./src/ofp/oxmfieldsid.cpp: oxm/oxmfields_compile3
	./oxm/oxmfields_compile3 > src/ofp/oxmfieldsdata.cpp


