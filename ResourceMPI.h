#pragma once

#include "DataTuples.h"
#include <vector>

struct DataTupleMPI
{
    int tupleID;
    int classID;
    int subjectID;
    int roomID;
    int teacher;
};

class DataTuplesMPI : public std::vector<DataTupleMPI>
{
  public:
    void pack(const DataTuples& tuples);
    void getIdVector(std::vector<int>& ids) const;
};

inline void
DataTuplesMPI::getIdVector(std::vector<int>& ids) const
{
    std::vector<DataTupleMPI>::const_iterator it;

    for (it = begin(); it != end(); ++it) {
        const DataTupleMPI& tuple = *it;
        ids.push_back(tuple.tupleID);
    }
}