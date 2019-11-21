all: *.h
	mpicxx -DSPRNG_MPI -DUSE_MPI --std=c++11 -Wall -g -o genetic *.cpp
