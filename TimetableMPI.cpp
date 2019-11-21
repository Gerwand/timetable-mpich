#include "TimetableMPI.h"

using namespace std;

void
TimetableMPI::pack(const Timetable* timetable)
{
    _genes.clear();
    _descriptor.clear();

    for (int i = 0; i < timetable->getPeriodsCount(); ++i) {
        const vector<int>& ids = (*timetable)[i].getTuplesIDs();
        _genes.insert(_genes.end(), ids.begin(), ids.end());
        _descriptor.push_back(ids.size());
    }

    _fitness = timetable->getFitness();
}

void
TimetableMPI::unpack(Timetable* dest)
{
    vector<int>::const_iterator itd;
    vector<int>::const_iterator start = _genes.begin(), end = _genes.begin();
    Periods::iterator itp = dest->getPeriods().begin();

    for (itd = _descriptor.begin(); itd != _descriptor.end(); ++itd, ++itp) {
        end += *itd;
        itp->getTuplesIDs() = vector<int>(start, end);
        start = end;
    }

    dest->setFitness(_fitness);
}
