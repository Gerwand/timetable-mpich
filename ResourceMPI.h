#pragma once

#include "DataTuples.h"
#include "mpi.h"
#include <stddef.h>
#include <vector>

struct DataTupleMPI
{
    int tupleID;
    int classID;
    int subjectID;
    int roomID;
    int teacherID;

    static const int numItems = 5;
};

class DataTuplesMPI : public std::vector<DataTupleMPI>
{
  public:
    void pack(const DataTuples& tuples);
    int getTotalIntElements();
    void getIdVector(std::vector<int>& ids) const;
};

inline int
DataTuplesMPI::getTotalIntElements()
{
    return DataTupleMPI::numItems * size();
}

inline void
DataTuplesMPI::getIdVector(std::vector<int>& ids) const
{
    std::vector<DataTupleMPI>::const_iterator it;

    for (it = begin(); it != end(); ++it) {
        const DataTupleMPI& tuple = *it;
        ids.push_back(tuple.tupleID);
    }
}