
WARNINGS = -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic

MAINS := $(wildcard ../src/ofp/*_main.cpp)
LIB_SOURCES := $(filter-out $(MAINS),$(wildcard ../src/ofp/*.cpp) $(wildcard ../src/ofp/impl/*.cpp))
LIB_OBJECTS := $(patsubst %.cpp,%.o,$(LIB_SOURCES))

BOOST_ROOT=/Users/bfish/code/ofp/external/boost_1_54_0
CPPFLAGS += -I ../include -stdlib=libc++ -isystem $(BOOST_ROOT)
CXXFLAGS += -g -std=c++11 $(WARNINGS)

# FIXME: which boost_system is this using?
LDLIBS += -lboost_system

all : nullagent nullcontroller

nullagent : nullagent_main.o $(LIB_OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDLIBS) $^ -o $@

nullcontroller : nullcontroller_main.o $(LIB_OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDLIBS) $^ -o $@

#ofp_test : $(TEST_OBJECTS) $(LIB_OBJECTS) ../src/ofp/oxmfields.o gtest_main.a
#	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lboost_system $^ -o $@
