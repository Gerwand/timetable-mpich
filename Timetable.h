#pragma once

#include "Period.h"

#include <algorithm>
#include <iostream>
#include <vector>

typedef std::vector<Period> Periods;

class Timetable
{
public:
  Timetable(int periodsNumber) { _periods.resize(periodsNumber); }

  Periods& getPeriods(int n) { return _periods; }
  const Periods& getPeriods(int n) const { return _periods; }
  int getPeriodsCount() const { return _periods.size(); }
  int getClashes(const DataTuples& tuples);

  void mutate();
  void removeDuplicates();
  void fillMissing(const std::vector<int>& ids);

  Period& operator[](int n) { return _periods[n]; }
  const Period& operator[](int n) const { return _periods[n]; }

  static int maxFitness;
private:
  Periods _periods;
};

std::ostream& operator<<(std::ostream& stream, const Timetable& timetable);