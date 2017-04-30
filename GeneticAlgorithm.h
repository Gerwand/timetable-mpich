#pragma once

#include "Timetable.h"

#include <vector>

typedef std::vector<Timetable> Timetables;

class GeneticAlgorithm
{
public:
  GeneticAlgorithm(size_t initialPopulation, const DataTuples& tuples);

  void getParents(const Timetable** parent1, const Timetable** parent2);

  static bool mate(const Timetable* parent1, const Timetable* parent2,
                   Timetable& child);

private:
  Timetables _population;
  const DataTuples& tuples;
};