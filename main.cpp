#include <fstream>
#include <iostream>

#include <vector>

#include "DataParser.h"
#include "Population.h"
#include "ResourceMPI.h"
#include "TimetableMPI.h"
#include "mpi.h"

using namespace std;

#define CHILD_NUM 1
#define CHILD_OPT 2
#define CHILD_DATA 3

typedef vector<TimetableMPI> TimetablesMPI;

void help(const string& name);

int
main(int argc, char* argv[])
{
    int generationSwapped = false;
    int numProcs, myID, master;
    int numTuples, numGenes, descSize;
    int initialPopulation = 10;
    int numChild;
    MPI_Comm world;
    MPI_Status status;
    DataResources resources;
    DataTuples tuples;
    DataTuplesMPI tuplesMPI;
    Population oldGeneration, newGeneration;
    Timetable* result;
    string inputFile;

    if (argc != 2) {
        help(argv[0]);
        return -1;
    }

    inputFile = argv[1];
    DataParser parser(inputFile);

    inputFile = argv[1];
    if (parser.getResources(resources, tuples)) {
        cout << "Error getting resources from file " << inputFile << "!"
             << endl;
        return -1;
    }

    tuplesMPI.pack(tuples);
    numTuples = tuplesMPI.size();

    MPI_Init(&argc, &argv);
    world = MPI_COMM_WORLD;
    MPI_Comm_size(world, &numProcs);
    MPI_Comm_rank(world, &myID);
    master = 0;

    // Send number of tuples
    MPI_Bcast(&numTuples, 1, MPI_INT, master, world);
    if (myID != master)
        tuplesMPI.resize(numTuples);
    // Send tuples
    MPI_Bcast(tuplesMPI.data(), tuplesMPI.getTotalIntElements(), MPI_INT,
              master, world);

    // Initialize population and send it to all workers
    if (myID == master) {
        oldGeneration.initRandom(initialPopulation);
    }

    // Send initial population
    for (int i = 0; i < initialPopulation; ++i) {
        TimetableMPI individual;
        if (myID == master) {
            individual.pack(oldGeneration.getPopulation()[i]);
            descSize = individual.getDescriptor().size();
            numGenes = individual.getGenes().size();
        }

        // Send size of descriptor and genes
        MPI_Bcast(&descSize, 1, MPI_INT, master, world);
        MPI_Bcast(&numGenes, 1, MPI_INT, master, world);
        if (myID != master)
            individual.setSize(numGenes, descSize);

        // Send packed individual
        MPI_Bcast(individual.getDescriptor().data(), descSize, MPI_INT, master,
                  world);
        MPI_Bcast(individual.getGenes().data(), numGenes, MPI_INT, master,
                  world);
        MPI_Bcast(individual.getFitness(), 1, MPI_INT, master, world);

        if (myID != master) {
            Timetable* unpacked = new Timetable(Population::periodsNumber);
            individual.unpack(unpacked);
            oldGeneration.addIndividual(unpacked);
        }
    }

    // Update natural selection factor
    if (myID == master)
        Population::naturalSelection = oldGeneration.getPopulationFitness();
    MPI_Bcast(&Population::naturalSelection, 1, MPI_INT, master, world);

    // Main loop of the algorithm

    // Master is responsible for gathering and updating generations
    if (myID == master) {
        do {

            // Create new generation
            for (int i = 0; i < numProcs - 1; ++i) {
                MPI_Recv(&numChild, 1, MPI_INT, MPI_ANY_SOURCE, CHILD_NUM,
                         world, &status);

                // Get all children from the node
                for (int i = 0; i < numChild; ++i) {
                    TimetableMPI individual;
                    MPI_Recv(&descSize, 1, MPI_INT, status.MPI_SOURCE,
                             CHILD_OPT, world, &status);
                    MPI_Recv(&numGenes, 1, MPI_INT, status.MPI_SOURCE,
                             CHILD_OPT, world, &status);

                    individual.setSize(numGenes, descSize);

                    // Send packed individual
                    MPI_Recv(individual.getDescriptor().data(), descSize,
                             MPI_INT, status.MPI_SOURCE, CHILD_OPT, world,
                             &status);
                    MPI_Recv(individual.getGenes().data(), numGenes, MPI_INT,
                             status.MPI_SOURCE, CHILD_OPT, world, &status);
                    MPI_Recv(individual.getFitness(), 1, MPI_INT,
                             status.MPI_SOURCE, CHILD_OPT, world, &status);

                    Timetable* unpacked =
                      new Timetable(Population::periodsNumber);
                    individual.unpack(unpacked);
                    newGeneration.addIndividual(unpacked);

                    if (unpacked->getFitness() == Population::maxFitness)
                        result = unpacked;
                }
            }

            // Investigate new generation
            if (newGeneration.hasExtincted()) {
                cout << "Populacja wymarla" << endl;
                Population::naturalSelection--;
            } else if (newGeneration.getPopulationFitness() >=
                       oldGeneration.getPopulationFitness()) {
                cout << "Podmieniam generacje i zwiekszam selekcje" << endl;
                Population::naturalSelection++;
                oldGeneration = newGeneration;
                generationSwapped = true;
            }

            // If new generation is not less fitter, broadcast it
            MPI_Bcast(&generationSwapped, 1, MPI_INT, master, world);
            if (generationSwapped) {
            }
            // Broadcast natural selection
            MPI_Bcast(&Population::naturalSelection, 1, MPI_INT, master, world);
        } while (1);
    }

    MPI_Finalize();

    // Print out result
    /*
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

    cout << "fitness: " << oldGeneration.getPopulationFitness() << endl <<
    endl;

    Population::naturalSelection =
    round(oldGeneration.getPopulationFitness());
    int i = 0;
    Timetable perfect(Population::periodsNumber);
    bool idealny = false;
    while (1) {
        cout << "Generation " << ++i
             << " population: " << oldGeneration.getPopulation().size() <<
    endl;

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
             << " natural selection: " << Population::naturalSelection <<
    endl
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
    */
}

void
help(const string& name)
{
    cout << "Usage of program: " << endl;
    cout << "\t" << name << " data_file" << endl;
}