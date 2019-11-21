#include "Period.h"

using namespace std;

int
Period::getClashes(const DataTuples& tuples)
{
    int clashes = 0;
    vector<int>::iterator it1, it2;
    for (it1 = _tuplesIDs.begin(); it1 != _tuplesIDs.end(); ++it1) {
        const DataTuple* first = tuples.get(*it1);
        for (it2 = it1 + 1; it2 != _tuplesIDs.end(); ++it2) {
            const DataTuple* second = tuples.get(*it2);

            if (first->getClass()->getId() == second->getClass()->getId())
                clashes++;

            if (first->getSubject()->getRoom()->getId() ==
                second->getSubject()->getRoom()->getId())
                clashes++;

            if (first->getSubject()->getTeacher()->getId() ==
                second->getSubject()->getTeacher()->getId())
                clashes++;
        }
    }

    return clashes;
}

int
Period::getClashes(const DataTuplesMPI& tuples)
{
    int clashes = 0;
    vector<int>::iterator it1, it2;
    for (it1 = _tuplesIDs.begin(); it1 != _tuplesIDs.end(); ++it1) {
        const DataTupleMPI* first = tuples.get(*it1);

        for (it2 = it1 + 1; it2 != _tuplesIDs.end(); ++it2) {
            const DataTupleMPI* second = tuples.get(*it2);

            if (first->classID == second->classID)
                clashes++;

            if (first->roomID == second->roomID)
                clashes++;

            if (first->teacherID == second->teacherID)
                clashes++;
        }
    }

    return clashes;
}