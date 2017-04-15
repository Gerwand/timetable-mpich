#pragma once

#include "Period.h"

#include <vector>

typedef std::vector<Period> Periods;

class Timetable
{
public:
  Timetable(int periodsNumber) { _periods.resize(periodsNumber); }

  Period& getPeriod(int n) { return _periods[n]; }
  int getClashes();

private:
  Periods _periods;
}

inline int
Timetable::getClashes()
{
  int totalClashes = 0;

  Periods::iterator it;

  for (it = _periods.begin(); it != _periods.end(); ++it)
    totalClashes += it->getClashes();

  return totalClashes;
}