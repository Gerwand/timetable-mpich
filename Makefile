CXXFLAGS  = --std=c++11 -Wall -g
CXXFLAGS += -DSPRNG_MPI -DUSE_MPI
CXX       = mpicxx
APP       = genetic
LFLAGS    = -lsprng
OBJ		  = CustomFunctions.o DataParser.o main.o
OBJ	     += Period.o Population.o Resource.o
OBJ	     += ResourceMPI.o Timetable.o TimetableMPI.o

${APP}: ${OBJ}
	${CXX} $^ -o $@ ${LFLAGS}

main.o: main.cpp DataParser.o Population.o TimetableMPI.o
	${CXX} ${CXXFLAGS} -c $< -o $@

Population.o: Population.cpp Population.h Timetable.o ResourceMPI.o
	${CXX} ${CXXFLAGS} -c $< -o $@

TimetableMPI.o: TimetableMPI.cpp TimetableMPI.h Timetable.o
	${CXX} ${CXXFLAGS} -c $< -o $@

Timetable.o: Timetable.cpp Timetable.h Period.o
	${CXX} ${CXXFLAGS} -c $< -o $@

Period.o: Period.cpp Period.h DataTuples.h Resource.o CustomFunctions.o ResourceMPI.o
	${CXX} ${CXXFLAGS} -c $< -o $@

ResourceMPI.o: ResourceMPI.cpp ResourceMPI.h DataTuples.h Resource.o
	${CXX} ${CXXFLAGS} -c $< -o $@

DataParser.o: DataParser.cpp DataParser.h DataTuples.h DataResources.h Resource.o CustomFunctions.o 
	${CXX} ${CXXFLAGS} -c $< -o $@

Resource.o: Resource.cpp Resource.h ContainerWrapper.h
	${CXX} ${CXXFLAGS} -c $< -o $@

CustomFunctions.o: CustomFunctions.cpp CustomFunctions.h
	${CXX} ${CXXFLAGS} -c $< -o $@


run:
	mpiexec -n 4 ./${APP} data.in data.out 25

clean:
	rm -f ${APP} data.out *.o