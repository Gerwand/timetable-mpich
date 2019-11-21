#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "CustomFunctions.h"
#include "DataResources.h"
#include "DataTuples.h"

class DataParser
{
  public:
    DataParser(std::string file) { _inputData.open(file); }
    ~DataParser() { _inputData.close(); }

    int getResources(DataResources& resources, DataTuples& tuples);

  private:
    enum DataType
    {
        UNKNOWN,
        CLASSES,
        TEACHERS,
        ROOMS,
        SUBJECTS,
        TUPLES
    };

    std::ifstream _inputData;
};