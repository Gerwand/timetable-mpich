#include "Period.h"

int
Period::getClashes()
{
  int clashes = 0;
  Tuples::iterator it1, it2;
  for (it1 = _tuples.begin(); it1 != _tuples.end(); ++it1) {
    const DataTuple* first = *it1;
    for (it2 = it1; it2 != _tuples.end(); ++it2) {
      const DataTuple* second = *it2;

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