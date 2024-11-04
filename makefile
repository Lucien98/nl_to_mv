.PHONY: all
all:
	mkdir -p build/objects
	mkdir -p bin
	g++ -std=c++11   -o build/objects/Benchmarks.o -c Benchmarks.cpp -DUNIX 
	g++  -std=c++11 -o ./bin/Benchmarks  build/objects/Benchmarks.o -lboost_program_options