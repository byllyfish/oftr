
MAINS := $(wildcard ../src/ofp/*_main.cpp)
LIB_SOURCES := $(filter-out $(MAINS),$(wildcard ../src/ofp/*.cpp) $(wildcard ../src/ofp/impl/*.cpp))
LIB_OBJECTS := $(patsubst %.cpp,%.o,$(LIB_SOURCES))

BOOST_ROOT = ../../external/boost_1_54_0_asio
BOOST_OBJ = $(BOOST_ROOT)/libs/system/src/error_code.o

CPPFLAGS += -I ../../include -isystem $(BOOST_ROOT) -isystem ../../external/yaml-io/include -std=c++11

ifeq ($(shell uname -s),Darwin)
 CPPFLAGS += -stdlib=libc++
 WARNINGS = -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-unused-parameter -Wno-documentation
 WARNINGS += -Wno-switch-enum -Wno-covered-switch-default
else
 WARNINGS = -Wall
endif

CXXFLAGS += -g $(WARNINGS)

LDFLAGS += -L ../.. -lofp
LDFLAGS += -L ../../external/yaml-io/ -lyamlio

all : nullagent nullcontroller

nullagent : nullagent_main.o 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

nullcontroller : nullcontroller_main.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) 
