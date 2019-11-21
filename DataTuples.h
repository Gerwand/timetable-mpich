#pragma once

#include "Resource.h"

class DataTuples : public ContainerWrapper<DataTuple>
{
  public:
	void getIdVector(std::vector<int> &ids) const;
};

inline void
DataTuples::getIdVector(std::vector<int> &ids) const
{
	std::vector<DataTuple *>::const_iterator it;

	for (it = getVector().begin(); it != getVector().end(); ++it)
	{
		DataTuple *tuple = *it;
		ids.push_back(tuple->getId());
	}
}