#include <fstream>
#include <iostream>

#include <vector>

#include "DataParser.h"
#include "Population.h"
#include "ResourceMPI.h"
#include "mpi.h"

using namespace std;

int
main(int argc, char* argv[])
{
    int numProcs, myID, master, server;
    MPI_Comm world;
    MPI_Status status;
    DataResources resources;
    DataTuples tuples;
    DataTuplesMPI tuplesMPI;

    srand(time(0));
    string inputFile = "data.in";

    DataParser parser(inputFile);

    if (parser.getResources(resources, tuples)) {
        cout << "Error getting resources from file " << inputFile << "!"
             << endl;
        return -1;
    }
    tuplesMPI.pack(tuples);

    MPI_Init(&argc, &argv);
    world = MPI_COMM_WORLD;
    MPI_Comm_size(world, &numProcs);
    MPI_Comm_rank(world, &myID);
    master = 0;
    server = numprocs - 1;

accepted


Jeremiah is right - MPI_Type_create_struct is the way to go here.

It's important to remember that MPI is a library, not built into the language; so it can't "see" what a structure looks like to serialize it by itself. So to send complex data types, you have to explicitly define its layout. In a language that does have native support for serialization, a set of MPI wrappers can concievably make use of that; mpi4py for instance makes use of python's pickle to transparently send complex data types; but in C, you have to roll up your sleeves and do it yourself.

For your structure, it looks like this:

#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct car_s
{
    int shifts;
    int topSpeed;
} car;

int main(int argc, char** argv)
{

    const int tag = 13;
    int size, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        fprintf(stderr, "Requires at least two processes.\n");
        exit(-1);
    }

    /* create a type for struct car */
    const int nitems = 2;
    int blocklengths[2] = { 1, 1 };
    MPI_Datatype types[2] = { MPI_INT, MPI_INT };
    MPI_Datatype mpi_car_type;
    MPI_Aint offsets[2];

    offsets[0] = offsetof(car, shifts);
    offsets[1] = offsetof(car, topSpeed);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_car_type);
    MPI_Type_commit(&mpi_car_type);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        car send;
        send.shifts = 4;
        send.topSpeed = 100;

        const int dest = 1;
        MPI_Send(&send, 1, mpi_car_type, dest, tag, MPI_COMM_WORLD);

        printf("Rank %d: sent structure car\n", rank);
    }
    if (rank == 1) {
        MPI_Status status;
        const int src = 0;

        car recv;

        MPI_Recv(&recv, 1, mpi_car_type, src, tag, MPI_COMM_WORLD, &status);
        printf("Rank %d: Received: shifts = %d topSpeed = %d\n", rank,
               recv.shifts, recv.topSpeed);
    }

    MPI_Type_free(&mpi_car_type);
    MPI_Finalize();




    MPI_Bcast(tuplesMPI.data(), tuplesMPI.size(), )

    /*
    cout << "Teachers:" << endl;
    cout << resources.getTeacher() << endl;
    cout << endl << "Rooms:" << endl;
    cout << resources.getRoom() << endl;
    cout << endl << "Subjects:" << endl;
    cout << resources.getSubject() << endl;
    cout << endl << "Classes:" << endl;
    cout << resources.getClass() << endl;

    cout << endl << "Tuples:" << endl;
    cout << tuples << endl;
    */
    vector<int> tuplesInd;
    tuples.getIdVector(tuplesInd);

    cout << endl << "Indexes: " << tuplesInd << endl;

    Population::setDataTuples(&tuples);
    Population oldGeneration;

    oldGeneration.initRandom(100);
    const Timetables& tt = oldGeneration.getPopulation();
    for (size_t i = 0; i < tt.size(); ++i) {
        cout << "Individual " << i << endl;
        cout << *(tt[i]) << endl;
        cout << "clashes: " << (*(tt[i])).getClashes(tuples) << endl;
    }

    cout << "fitness: " << oldGeneration.getPopulationFitness() << endl << endl;

    Population::naturalSelection = round(oldGeneration.getPopulationFitness());
    int i = 0;
    Timetable perfect(Population::periodsNumber);
    bool idealny = false;
    while (1) {
        cout << "Generation " << ++i
             << " population: " << oldGeneration.getPopulation().size() << endl;

        Population newGeneration;
        for (int i = 0; i < 1000; ++i) {
            Timetable *parent1, *parent2;
            oldGeneration.getParents(parent1, parent2);
            Timetable* child = new Timetable(Population::periodsNumber);

            bool alive = Population::mate(parent1, parent2, *child);
            if (alive) {
                newGeneration.addIndividual(child);
            }
            if (child->getFitness() == Population::maxFitness) {
                cout << "Mamy idealny okaz!" << endl;
                perfect = *child;
                idealny = true;
                break;
            }
        }
        if (idealny)
            break;

        if (newGeneration.hasExtincted()) {
            cout << "Populacja wymarla" << endl;
            Population::naturalSelection--;
            --i;
            continue;
        }

        cout << "fitness: " << newGeneration.getPopulationFitness()
             << " natural selection: " << Population::naturalSelection << endl
             << endl;

        if (newGeneration.getPopulationFitness() >=
            oldGeneration.getPopulationFitness()) {
            cout << "Pomieniam generacje i zwiekszam selekcje" << endl;
            Population::naturalSelection++;
            oldGeneration = newGeneration;
        } else {
            --i;
        }
    }
}
