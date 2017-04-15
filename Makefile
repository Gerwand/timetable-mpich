all: *.h
	g++ --std=c++11 -Wall -g -o genetic *.cpp
