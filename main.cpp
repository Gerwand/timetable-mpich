#include <fstream>
#include <iostream>

#include <vector>

#include "DataParser.h"
#include "Population.h"

using namespace std;

int main()
{
	srand(time(0));
	string inputFile = "data.in";

	DataResources resources;
	DataTuples tuples;

	DataParser parser(inputFile);

	if (parser.getResources(resources, tuples))
	{
		cout << "Error getting resources from file " << inputFile << "!" << endl;
		return -1;
	}

	cout << "Teachers:" << endl;
	cout << resources.getTeacher() << endl;
	cout << endl
		 << "Rooms:" << endl;
	cout << resources.getRoom() << endl;
	cout << endl
		 << "Subjects:" << endl;
	cout << resources.getSubject() << endl;
	cout << endl
		 << "Classes:" << endl;
	cout << resources.getClass() << endl;

	cout << endl
		 << "Tuples:" << endl;
	cout << tuples << endl;

	vector<int> tuplesInd;
	tuples.getIdVector(tuplesInd);

	cout << endl
		 << "Indexes: " << tuplesInd << endl;

	Population::setDataTuples(&tuples);
	Population oldGeneration;

	oldGeneration.initRandom(10);
	const Timetables &tt = oldGeneration.getPopulation();
	for (size_t i = 0; i < tt.size(); ++i)
	{
		cout << "Individual " << i << endl;
		cout << *(tt[i]) << endl;
		cout << "clashes: " << (*(tt[i])).getClashes(tuples) << endl;
	}

	cout << "fitness: " << oldGeneration.getPopulationFitness() << endl
		 << endl;

	Population newGeneration;
	for (int i = 0; i < 100; ++i)
	{
		Timetable *parent1, *parent2;
		oldGeneration.getParents(parent1, parent2);
		Timetable *child = new Timetable(parent1->getPeriodsCount());

		bool alive = Population::mate(parent1, parent2, *child);
		if (alive)
			newGeneration.addIndividual(child);
		if (child->getFitness() == Population::maxFitness)
			cout << "Mamy idealny okaz!" << endl;
	}

	
  //const Timetables& ng = newGeneration.getPopulation();

  /*
  for (size_t i = 0; i < ng.size(); ++i) {
    cout << "Individual " << i << endl;
    cout << *(ng[i]) << endl;
    cout << "clashes: " << (*(ng[i])).getClashes(tuples) << endl;
  }
*/
	if (newGeneration.hasExtincted())
		cout << "Populacja wymarla" << endl;
	else
		cout << "fitness: " << newGeneration.getPopulationFitness() << endl
			 << endl;

	oldGeneration = newGeneration;
}
