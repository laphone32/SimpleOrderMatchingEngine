all:
	g++ -Iinclude -Isrc -isystem -Wall -O3 -g -lrt -std=c++14 -o testDemo src/Dummy.cpp
