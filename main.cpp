#include <fstream>
#include <iostream>

#include <vector>

#include "DataParser.h"
#include "Population.h"
#include "ResourceMPI.h"
#include "TimetableMPI.h"
#include "mpi.h"
#include "sprng_cpp.h"

using namespace std;

#define CHILD_NUM 1
#define CHILD_OPT 2
#define CHILD_DATA 3

#define BIRTH 1000
#define MAX_FIT 100

#define GEN_TYPE 0

typedef vector<TimetableMPI> TimetablesMPI;

void help(const string& name);

int
main(int argc, char* argv[])
{
    int generationSwapped = false;
    int numProcs, myID, master;
    int streamID, numStreams, seed;
    int numTuples, numGenes, descSize, numPeriods;
    int initialPopulation = 1000;
    int numChild, numIndividual;
    int finished;
    int populationPerProcess = BIRTH;
    int n = 0;
    int rc = 0;
    MPI_Comm world;
    MPI_Status status;
    DataResources resources;
    DataTuples tuples;
    DataTuplesMPI tuplesMPI;
    Population oldGeneration, newGeneration;
    string inputFile;
    Timetable* result = nullptr;
    Sprng* stream;

    // There should be at least 3 arguments
    if (argc != 3) {
        help(argv[0]);
        return -1;
    }

    ///////////////////////////////////////////////////////////////////////////
    //                            MPI Initialization
    ///////////////////////////////////////////////////////////////////////////
    MPI_Init(&argc, &argv);
    world = MPI_COMM_WORLD;
    MPI_Comm_size(world, &numProcs);
    MPI_Comm_rank(world, &myID);
    master = 0;

    ///////////////////////////////////////////////////////////////////////////
    //                              Data Parsing
    ///////////////////////////////////////////////////////////////////////////
    if (myID == master) {
        inputFile = argv[1];
        DataParser parser(inputFile);

        if (parser.getResources(resources, tuples)) {
            cout << "Error getting resources from file " << inputFile << "!"
                 << endl;
            rc = -1;
        }
        tuplesMPI.pack(tuples);
        numTuples = tuplesMPI.size();
        sscanf(argv[2], "%d", &numPeriods);
    }

    MPI_Bcast(&rc, 1, MPI_INT, master, world);
    if (rc) {
        MPI_Finalize();
        return rc;
    }
    MPI_Bcast(&numPeriods, 1, MPI_INT, master, world);

    ///////////////////////////////////////////////////////////////////////////
    //                            sprng Initialization
    ///////////////////////////////////////////////////////////////////////////
    streamID = myID;
    numStreams = numProcs;
    seed = make_sprng_seed();

    stream = SelectType(GEN_TYPE);
    stream->init_sprng(streamID, numStreams, seed, SPRNG_DEFAULT);

    ///////////////////////////////////////////////////////////////////////////
    //                   Broadcast ID representation of objects
    ///////////////////////////////////////////////////////////////////////////

    // Send number of tuples
    MPI_Bcast(&numTuples, 1, MPI_INT, master, world);
    if (myID != master)
        tuplesMPI.resize(numTuples);
    // Send tuples
    MPI_Bcast(tuplesMPI.data(), tuplesMPI.getTotalIntElements(), MPI_INT,
              master, world);

    ///////////////////////////////////////////////////////////////////////////
    //                          Population configuration
    ///////////////////////////////////////////////////////////////////////////
    Population::setDataTuples(&tuplesMPI);
    Population::configure(MAX_FIT, numPeriods, stream);

    ///////////////////////////////////////////////////////////////////////////
    //                        Start with random population
    ///////////////////////////////////////////////////////////////////////////
    // Initialize population and send it to all workers
    if (myID == master) {
        oldGeneration.initRandom(initialPopulation);
    }

    ///////////////////////////////////////////////////////////////////////////
    //                    Broadcast population to all processes
    ///////////////////////////////////////////////////////////////////////////
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

    ///////////////////////////////////////////////////////////////////////////
    //                         Main loop - server side
    ///////////////////////////////////////////////////////////////////////////
    // Master is responsible for gathering and updating generations
    if (myID == master) {

        do {
            newGeneration.clear();
            ///////////////////////////////////////////////////////////////////
            //                     Gather children from each process
            ///////////////////////////////////////////////////////////////////
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

                    if (unpacked->getFitness() == Population::maxFitness) {
                        cout << "Proc " << myID << ": "
                             << "Perfect! " << unpacked->getFitness() << endl;

                        result = unpacked;
                    }
                }
            }
            ///////////////////////////////////////////////////////////////////
            //                      Completion check
            ///////////////////////////////////////////////////////////////////

            // If result found, inform all processes and exit
            finished = result != nullptr;
            MPI_Bcast(&finished, 1, MPI_INT, master, world);
            if (finished)
                break;

            cout << "Generacja " << n
                 << " fitness: " << newGeneration.getPopulationFitness()
                 << endl;
            ///////////////////////////////////////////////////////////////////
            //                     Generation investigation
            ///////////////////////////////////////////////////////////////////
            if (newGeneration.hasExtincted()) {
                cout << "Populacja wymarla - zmniejszam wsp. selekcji" << endl;
                Population::naturalSelection--;
            } else if (newGeneration.getPopulationFitness() >=
                       oldGeneration.getPopulationFitness()) {
                cout << "Podmieniam generacje i zwiekszam wsp. selekcji"
                     << endl;
                Population::naturalSelection++;
                oldGeneration = newGeneration;
                ++n;
                generationSwapped = true;
            } else {
                cout
                  << "Nowa populacja gorzej przystosowana, zostaje przy starej"
                  << endl;
            }
            cout << endl;

            ///////////////////////////////////////////////////////////////////
            //                     Broadcast generation
            ///////////////////////////////////////////////////////////////////
            // If new generation is fitter or the same, broadcast it
            MPI_Bcast(&generationSwapped, 1, MPI_INT, master, world);
            if (generationSwapped) {
                numIndividual = newGeneration.getPopulation().size();
                MPI_Bcast(&numIndividual, 1, MPI_INT, master, world);

                for (int i = 0; i < numIndividual; ++i) {
                    TimetableMPI individual;

                    individual.pack(oldGeneration.getPopulation()[i]);
                    descSize = individual.getDescriptor().size();
                    numGenes = individual.getGenes().size();

                    // Send size of descriptor and genes
                    MPI_Bcast(&descSize, 1, MPI_INT, master, world);
                    MPI_Bcast(&numGenes, 1, MPI_INT, master, world);

                    // Send packed individual
                    MPI_Bcast(individual.getDescriptor().data(), descSize,
                              MPI_INT, master, world);
                    MPI_Bcast(individual.getGenes().data(), numGenes, MPI_INT,
                              master, world);
                    MPI_Bcast(individual.getFitness(), 1, MPI_INT, master,
                              world);
                }
            }

            generationSwapped = false;
            // Broadcast natural selection factor
            MPI_Bcast(&Population::naturalSelection, 1, MPI_INT, master, world);
        } while (1);

    } else {
        ///////////////////////////////////////////////////////////////////////
        //                         Main loop - worker side
        ///////////////////////////////////////////////////////////////////////

        do {
            newGeneration.clear();
            ///////////////////////////////////////////////////////////////////
            //                     Generate children
            ///////////////////////////////////////////////////////////////////
            // Mate proper children in process
            for (int i = 0; i < populationPerProcess; ++i) {
                Timetable *parent1, *parent2;
                oldGeneration.getParents(parent1, parent2);
                Timetable* child = new Timetable(Population::periodsNumber);

                bool alive = Population::mate(parent1, parent2, *child);
                if (alive) {
                    newGeneration.addIndividual(child);
                }
            }

            ///////////////////////////////////////////////////////////////////
            //                     Send all alive children
            ///////////////////////////////////////////////////////////////////
            numChild = newGeneration.getPopulation().size();
            MPI_Send(&numChild, 1, MPI_INT, master, CHILD_NUM, world);

            // Send all children from the node
            for (int i = 0; i < numChild; ++i) {
                TimetableMPI individual;
                individual.pack(newGeneration.getPopulation()[i]);
                descSize = individual.getDescriptor().size();
                numGenes = individual.getGenes().size();
                MPI_Send(&descSize, 1, MPI_INT, master, CHILD_OPT, world);
                MPI_Send(&numGenes, 1, MPI_INT, master, CHILD_OPT, world);

                // Send packed individual
                MPI_Send(individual.getDescriptor().data(), descSize, MPI_INT,
                         master, CHILD_OPT, world);
                MPI_Send(individual.getGenes().data(), numGenes, MPI_INT,
                         master, CHILD_OPT, world);
                MPI_Send(individual.getFitness(), 1, MPI_INT, master, CHILD_OPT,
                         world);
            }

            ///////////////////////////////////////////////////////////////////
            //                        Completion check
            ///////////////////////////////////////////////////////////////////
            MPI_Bcast(&finished, 1, MPI_INT, master, world);
            if (finished)
                break;

            ///////////////////////////////////////////////////////////////////
            //                        Generation update
            ///////////////////////////////////////////////////////////////////
            MPI_Bcast(&generationSwapped, 1, MPI_INT, master, world);
            if (generationSwapped) {
                oldGeneration.clear();
                MPI_Bcast(&numIndividual, 1, MPI_INT, master, world);

                for (int i = 0; i < numIndividual; ++i) {
                    TimetableMPI individual;

                    // Receive size of descriptor and genes
                    MPI_Bcast(&descSize, 1, MPI_INT, master, world);
                    MPI_Bcast(&numGenes, 1, MPI_INT, master, world);

                    individual.setSize(numGenes, descSize);

                    // Send packed individual
                    MPI_Bcast(individual.getDescriptor().data(), descSize,
                              MPI_INT, master, world);
                    MPI_Bcast(individual.getGenes().data(), numGenes, MPI_INT,
                              master, world);
                    MPI_Bcast(individual.getFitness(), 1, MPI_INT, master,
                              world);

                    Timetable* unpacked =
                      new Timetable(Population::periodsNumber);
                    individual.unpack(unpacked);
                    oldGeneration.addIndividual(unpacked);
                }
            }

            // Broadcast natural selection factor
            MPI_Bcast(&Population::naturalSelection, 1, MPI_INT, master, world);
        } while (1);
    }

    ///////////////////////////////////////////////////////////////////////////
    //                                  Result
    ///////////////////////////////////////////////////////////////////////////
    if (myID == master) {
        cout << *result << endl;
        result->printPretty(cout, tuples);
    }

    stream->free_sprng();
    MPI_Finalize();
}

void
help(const string& name)
{
    cout << "Usage of program: " << endl;
    cout << "\t" << name << " data_file periods_number" << endl;
}