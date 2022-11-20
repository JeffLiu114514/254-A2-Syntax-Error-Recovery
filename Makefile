# Trivial makefile for the calculator scanner/parser.
# Michael L. Scott, 2008-2022.

# Note that the rule for the goal (parse)
# must be the first one in this file.

CPP = g++
CPPFLAGS = -std=c++17 -g -O2 -Wall -Wpedantic

.cpp.o:
	$(CPP) $(CPPFLAGS) -c $<

parse: parse.o scan.o
	$(CPP) $(CPPFLAGS) -o parse parse.o scan.o

clean:
	-rm -f *.o parse

parse.o: scan.hpp
scan.o: scan.hpp


