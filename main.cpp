#include <fstream>
#include <iostream>

#include <vector>

#include "DataParser.h"
#include "CustomFunctions.h"

using namespace std;

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


}
