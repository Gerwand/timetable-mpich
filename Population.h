#pragma once

#include "Timetable.h"

#include <vector>

typedef std::vector<Timetable*> Timetables;

class Population
{
  public:
    Population(const Population& population);
    Population(const Timetables& timetables);
    Population(){};
    ~Population();

    const Population& operator=(const Population& population);
    const Population& operator=(const Timetables& timetables);

    const Timetables& getPopulation() { return _timetables; }

    void addIndividual(Timetable* individual);
    void initRandom(size_t size);
    bool hasExtincted() { return _timetables.size() < minimumPopulationSize; }
    void clear();

    static void setDataTuples(const DataTuples* tuples);
    void getParents(Timetable*& parent1, Timetable*& parent2);
    static bool mate(const Timetable* parent1, const Timetable* parent2,
                     Timetable& child);

    double getPopulationFitness();
    void printPopulation();

    static int maxFitness;
    static int naturalSelection;
    static int periodsNumber;
    static size_t minimumPopulationSize;

  private:
    Timetables _timetables;
    static const DataTuples* _tuples;
    static std::vector<int> _tuplesInd;
};

inline Population::Population(const Population& population)
{
    *this = population;
}

inline Population::Population(const Timetables& timetables)
{
    *this = timetables;
}

inline Population::~Population()
{
    clear();
}

inline const Population&
Population::operator=(const Population& population)
{
    clear();
    const Timetables& timetables = population._timetables;

    Timetables::const_iterator it;
    for (it = timetables.begin(); it != timetables.end(); ++it) {
        _timetables.push_back(new Timetable(**it));
    }

    return *this;
}

inline const Population&
Population::operator=(const Timetables& timetables)
{
    clear();
    _timetables = timetables;

    return *this;
}

inline void
Population::clear()
{
    Timetables::iterator it;
    for (it = _timetables.begin(); it != _timetables.end(); ++it)
        delete *it;

    _timetables.clear();
}

inline void
Population::addIndividual(Timetable* individual)
{
    _timetables.push_back(individual);
}

inline void
Population::setDataTuples(const DataTuples* tuples)
{
    _tuples = tuples;
    tuples->getIdVector(_tuplesInd);
}

inline double
Population::getPopulationFitness()
{
    Timetables::iterator it;
    double totalFitness = 0.0;
    for (it = _timetables.begin(); it != _timetables.end(); ++it)
        totalFitness += (*it)->getFitness();

    return totalFitness /= _timetables.size();
}