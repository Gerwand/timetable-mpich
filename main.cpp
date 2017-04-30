#include <fstream>
#include <iostream>

#include <vector>

#include "CustomFunctions.h"
#include "DataParser.h"
#include "Timetable.h"

using namespace std;

typedef std::vector<Timetable> Timetables;

int
main()
{
  string inputFile = "data.in";

  DataResources resources;
  DataTuples tuples;

  DataParser parser(inputFile);

  if (parser.getResources(resources, tuples)) {
    cout << "Error getting resources from file " << inputFile << "!" << endl;
    return -1;
  }

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

  vector<int> tuplesInd;
  tuples.getIdVector(tuplesInd);

  cout << endl << "Indexes: " << tuplesInd << endl;

  Timetables population;
  size_t size = 10;
  size_t nperiods = 30;
  size_t tuplesPerPeriod = tuplesInd.size() / (nperiods);
  size_t rest = tuplesInd.size() - nperiods * tuplesPerPeriod;
  for (size_t i = 0; i < size; ++i) {
    vector<int> curTuples = tuplesInd;
    vector<int>::iterator it = curTuples.begin();
    random_shuffle(curTuples.begin(), curTuples.end());

    population.push_back(Timetable(nperiods));
    Timetable& timetable = population.back();
    Period* period = nullptr;
    int n = -1;
    for (size_t k = 0; k < nperiods * tuplesPerPeriod; ++k) {
      if (k % tuplesPerPeriod == 0) {
        ++n;
        period = &(timetable[n]);
        if (period == nullptr)
          period = &(timetable[n - 1]);
      }

      period->addTuple(*it);
      ++it;
    }

    for (size_t k = 0; k < rest; ++k) {
      period = &(timetable[k]);

      period->addTuple(*it);
      ++it;
    }
  }

  cout << population[0] << endl;
}
