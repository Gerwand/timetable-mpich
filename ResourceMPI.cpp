#include "ResourceMPI.h"

void
DataTuplesMPI::pack(const DataTuples& tuples)
{
    clear();
    const std::vector<DataTuple*> tupVect = tuples.getVector();

    std::vector<DataTuple*>::const_iterator it;
    for (it = tupVect.begin(); it != tupVect.end(); ++it) {
        const DataTuple* tuple = *it;
        const Subject* subject = tuple->getSubject();

        push_back({ tuple->getId(), tuple->getClass()->getId(),
                    subject->getId(), subject->getTeacher()->getId(),
                    subject->getRoom()->getId() });
    }
}