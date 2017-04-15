#pragma once

#include "Resource.h"
#include <list>

typedef std::list<const DataTuple *> Tuples;

class Period
{
public:
	const Tuples& getTuples() { return _tuples; }
	void addTuple(const DataTuple *tuple) { _tuples.push_back(tuple); }

	int getClashes();
private:
	Tuples _tuples;
};
