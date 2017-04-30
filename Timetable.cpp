#include "Timetable.h"

using namespace std;

int
Timetable::getClashes(const DataTuples& tuples)
{
  int totalClashes = 0;

  Periods::iterator it;

  for (it = _periods.begin(); it != _periods.end(); ++it)
    totalClashes += it->getClashes(tuples);

  return totalClashes;
}

void
Timetable::removeDuplicates()
{
  vector<int> foundIDs;

  Periods::iterator itp;
  for (itp = _periods.begin(); itp != _periods.end(); ++itp) {
    vector<int>& tuples = itp->getTuplesIDs();

    vector<int>::iterator itt;
    for (itt = tuples.begin(); itt != tuples.end(); ++itt) {
      int id = *itt;

      auto found = std::find(foundIDs.begin(), foundIDs.end(), id);

      if (found != foundIDs.end()) {
        tuples.erase(itt);
        --itt;
      } else {
        foundIDs.push_back(id);
      }
    }
  }
}

void
Timetable::fillMissing(const std::vector<int>& ids)
{
  vector<int> foundIDs;

  Periods::iterator itp;
  for (itp = _periods.begin(); itp != _periods.end(); ++itp) {
    vector<int>& tuples = itp->getTuplesIDs();

    vector<int>::iterator itt;
    for (itt = tuples.begin(); itt != tuples.end(); ++itt) {
      int id = *itt;

      auto found = std::find(foundIDs.begin(), foundIDs.end(), id);

      if (found != foundIDs.end()) {
        tuples.erase(itt);
        --itt;
      } else {
        foundIDs.push_back(id);
        // RANDOM
        int targetPeriod = rand() % _periods.size();
        _periods[targetPeriod].addTuple(id);
      }
    }
  }
}

void
Timetable::mutate()
{
  int srcPeriod = rand() % _periods.size();
  int dstPeriod = rand() % _periods.size();

  if (srcPeriod == dstPeriod)
      return;

  vector<int>& genes = _periods[srcPeriod].getTuplesIDs();
  int srcGene = rand() % genes.size();

  int gene = genes[srcGene];

  _periods[dstPeriod].addTuple(gene);
}

std::ostream&
operator<<(std::ostream& stream, const Timetable& timetable)
{
  for (int i = 0; i < timetable.getPeriodsCount(); ++i) {
    stream << i << ": " << timetable[i] << endl;
  }

  return stream << endl;
}