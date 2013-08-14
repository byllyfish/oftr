
MAINS := $(wildcard ../src/ofp/*_main.cpp)
LIB_SOURCES := $(filter-out $(MAINS),$(wildcard ../src/ofp/*.cpp) $(wildcard ../src/ofp/impl/*.cpp))
LIB_OBJECTS := $(patsubst %.cpp,%.o,$(LIB_SOURCES))

BOOST_ROOT = ../external/boost_1_54_0_asio
BOOST_OBJ = $(BOOST_ROOT)/libs/system/src/error_code.o

CPPFLAGS += -I ../include -isystem $(BOOST_ROOT) -std=c++11

ifeq ($(shell uname -s),Darwin)
 CPPFLAGS += -stdlib=libc++
 WARNINGS = -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-unused-parameter
else
 WARNINGS = -Wall
endif

CXXFLAGS += -g $(WARNINGS)

all : nullagent nullcontroller

nullagent : nullagent_main.o $(LIB_OBJECTS) $(BOOST_OBJ)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

nullcontroller : nullcontroller_main.o $(LIB_OBJECTS) $(BOOST_OBJ)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@
