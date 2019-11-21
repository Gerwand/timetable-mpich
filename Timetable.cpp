#include "Timetable.h"

using namespace std;

Sprng* Timetable::_stream;

int
Timetable::getClashes(const DataTuples& tuples)
{
    int totalClashes = 0;

    Periods::iterator it;

    for (it = _periods.begin(); it != _periods.end(); ++it)
        totalClashes += it->getClashes(tuples);

    return totalClashes;
}

int
Timetable::getClashes(const DataTuplesMPI& tuples)
{
    int totalClashes = 0;

    Periods::iterator it;

    for (it = _periods.begin(); it != _periods.end(); ++it)
        totalClashes += it->getClashes(tuples);

    return totalClashes;
}

void
Timetable::removeDuplicates(const std::vector<int>& ids)
{
    vector<vector<int>> foundIDs;
    foundIDs.resize(ids.size());
    Periods::iterator itp;
    int i = 0;
    for (itp = _periods.begin(); itp != _periods.end(); ++itp, ++i) {
        vector<int>& tuples = itp->getTuplesIDs();

        vector<int>::iterator itt;
        for (itt = tuples.begin(); itt != tuples.end(); ++itt)
            foundIDs[(*itt) - 1].push_back(i);
    }

    for (size_t l = 0; l < foundIDs.size(); ++l) {
        int occurences = foundIDs[l].size();
        if (occurences <= 1)
            continue;

        int indRemove = _stream->isprng() % occurences;
        int nPeriod = foundIDs[l][indRemove];
        vector<int>& tuples = _periods[nPeriod].getTuplesIDs();
        auto toRemove = find(tuples.begin(), tuples.end(), l + 1);

        tuples.erase(toRemove);
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
            foundIDs.push_back(id);
        }
    }

    if (ids.size() == foundIDs.size())
        return;

    sort(foundIDs.begin(), foundIDs.end());
    vector<int>::const_iterator iti;
    for (iti = ids.begin(); iti != ids.end(); ++iti) {
        int id = *iti;
        auto found = lower_bound(foundIDs.begin(), foundIDs.end(), id);

        if (found == foundIDs.end() || *found != id) {
            // RAND
            int size = _periods.size();
            int targetPeriod = _stream->isprng() % size;
            _periods[targetPeriod].addTuple(id);
        }
    }
}
#include <assert.h>
void
Timetable::mutate()
{
    int srcPeriod;
    int dstPeriod;
    int tries = 10;
    vector<int>* genes;
    do {
        int n1 = _stream->isprng();
        int n2 = _stream->isprng();

        srcPeriod = n1 % _periods.size();
        dstPeriod = n2 % _periods.size();
        genes = &_periods[srcPeriod].getTuplesIDs();
        if (!(--tries))
            break;
    } while (srcPeriod == dstPeriod || genes->empty());

    if (srcPeriod == dstPeriod || genes->empty())
        return;

    int srcGene = _stream->isprng() % genes->size();

    int gene = (*genes)[srcGene];

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

void
Timetable::printPretty(std::ostream& stream, const DataTuples& tuples)
{
    Periods::iterator itp;
    int i = 0;

    for (itp = _periods.begin(); itp != _periods.end(); ++itp, ++i) {
        stream << "#Period " << i << ": " << endl;

        const vector<int>& ids = itp->getTuplesIDs();
        vector<int>::const_iterator iti;
        for (iti = ids.begin(); iti != ids.end(); ++iti) {
            const DataTuple* tuple = tuples.get(*iti);
            const Class* classObj = tuple->getClass();
            const Subject* subject = tuple->getSubject();
            const Teacher* teacher = subject->getTeacher();
            const Room* room = subject->getRoom();

            stream << "\tClass: " << classObj->getName()
                   << ", Subject: " << subject->getName()
                   << ", Teacher: " << teacher->getName()
                   << ", Room: " << room->getName() << endl;
        }
    }
}