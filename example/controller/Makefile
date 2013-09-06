
CPPFLAGS += -I ../../include -std=c++11
CXXFLAGS += -Wall
LDFLAGS  += -L ../.. -lofp

ifeq ($(shell uname -s),Darwin)
 CPPFLAGS += -stdlib=libc++
endif

OBJECTS = simple_main.o simplechannellistener.o simplecontroller.o

simple : $(OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm $(OBJECTS)
