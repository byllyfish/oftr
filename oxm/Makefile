# Makefile for oxmfields.h and oxmfields.cpp
# This Makefile is designed to be called by the top-level make file with
# exported options.

oxmfields: ../include/ofp/oxmfields.h ../src/ofp/oxmfields.cpp

oxmfields_compile: $(OXM_OBJECTS) oxmfields_compile_main.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

oxmfields_compile2: $(OXM_OBJECTS) oxmfields_main.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

oxmfields_main.cpp: oxmfields_compile oxmfields.tab
	./oxmfields_compile < oxmfields.tab > $@

../include/ofp/oxmfields.h: oxmfields_compile oxmfields.tab
	./oxmfields_compile -h < oxmfields.tab > ../include/ofp/oxmfields.h

../src/ofp/oxmfields.cpp: oxmfields_compile2
	./oxmfields_compile2 > ../src/ofp/oxmfields.cpp

.PHONY: clean
clean:
	rm -f *.o
	rm -f oxmfields_compile oxmfields_compile2