#include "Population.h"

using namespace std;

const DataTuples *Population::_tuples;
std::vector<int> Population::_tuplesInd;
int Population::maxFitness = 50;
int Population::naturalSelection = 10;

void Population::initRandom(size_t size)
{
	size_t nperiods = 30;
	size_t tuplesPerPeriod = _tuplesInd.size() / (nperiods);
	size_t rest = _tuplesInd.size() - nperiods * tuplesPerPeriod;
	for (size_t i = 0; i < size; ++i)
	{
		vector<int> curTuples = _tuplesInd;
		vector<int>::iterator it = curTuples.begin();
		random_shuffle(curTuples.begin(), curTuples.end());

		_timetables.push_back(new Timetable(nperiods));
		Timetable &timetable = *_timetables.back();
		Period *period = nullptr;
		int n = -1;
		for (size_t k = 0; k < nperiods * tuplesPerPeriod; ++k)
		{
			if (k % tuplesPerPeriod == 0)
			{
				++n;
				period = &(timetable[n]);
				if (period == nullptr)
					period = &(timetable[n - 1]);
			}

			period->addTuple(*it);
			++it;
		}

		for (size_t k = 0; k < rest; ++k)
		{
			period = &(timetable[k]);

			period->addTuple(*it);
			++it;
		}
		int fitness = maxFitness - timetable.getClashes(*_tuples);
		timetable.setFitness(fitness);
	}
}

void Population::getParents(Timetable *&parent1, Timetable *&parent2)
{
	// RANDOM
	int p1 = rand() % _timetables.size();
	int p2;
	do
	{
		p2 = rand() % _timetables.size();
	} while (p1 == p2);

	parent1 = _timetables[p1];
	parent2 = _timetables[p2];
}

bool Population::mate(const Timetable *parent1, const Timetable *parent2,
					  Timetable &child)
{
	const Periods &periods1 = parent1->getPeriods();
	const Periods &periods2 = parent2->getPeriods();
	Periods &periodsc = child.getPeriods();

	// Copy genes
	Periods::const_iterator itp1, itp2;
	Periods::iterator itc;
	for (itp1 = periods1.begin(), itp2 = periods2.begin(), itc = periodsc.begin();
		 itp1 != periods1.end(); ++itp1, ++itp2, ++itc)
	{
		int maxSize = max(itp1->getTuplesCount(), itp2->getTuplesCount());
		// RAND
		size_t crossoverSite = rand() % maxSize;

		const vector<int> &idp1 = itp1->getTuplesIDs();
		const vector<int> &idp2 = itp2->getTuplesIDs();
		vector<int> &idc = itc->getTuplesIDs();

		for (size_t i = 0; i < crossoverSite && i < idp1.size(); ++i)
			idc.push_back(idp1[i]);
		for (size_t i = crossoverSite; i < idp2.size(); ++i)
			idc.push_back(idp2[i]);
	}

	// Mutate
	child.mutate();
	// Remove duplicates
	child.removeDuplicates();
	// Restore state
	child.fillMissing(_tuplesInd);

	int clashes = child.getClashes(*_tuples);
	int fitness = maxFitness - clashes;
	child.setFitness(fitness);

	return fitness - naturalSelection >= 0;
}
