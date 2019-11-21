#pragma once

#include "CustomFunctions.h"
#include "DataTuples.h"
#include "Resource.h"
#include <list>

typedef std::list<const DataTuple *> Tuples;

class Period
{
  public:
	std::vector<int> &getTuplesIDs() { return _tuplesIDs; }
	const std::vector<int> &getTuplesIDs() const { return _tuplesIDs; }

	void addTuple(int tupleID) { _tuplesIDs.push_back(tupleID); }

	int getTuplesCount() const { return _tuplesIDs.size(); }
	int getClashes(const DataTuples &tuples);

  private:
	std::vector<int> _tuplesIDs;
};

inline std::ostream &
operator<<(std::ostream &stream, const Period &period)
{
	return stream << period.getTuplesIDs();
}