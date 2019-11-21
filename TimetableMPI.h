#pragma once

#include "Timetable.h"
#include "mpi.h"

class TimetableMPI
{
  public:
    void pack(const Timetable* timetable);
    void unpack(Timetable* dest);
    void setSize(int nGenes, int nDesc);

    std::vector<int>& getGenes() { return _genes; }
    std::vector<int>& getDescriptor() { return _descriptor; }
    const std::vector<int>& getGenes() const { return _genes; }
    const std::vector<int>& getDescriptor() const { return _descriptor; }
    int* getFitness() { return &_fitness; }

  private:
    std::vector<int> _genes;
    std::vector<int> _descriptor;
    int _fitness;
};

inline void
TimetableMPI::setSize(int nGenes, int nDesc)
{
    _genes.resize(nGenes);
    _descriptor.resize(nDesc);
}