#pragma once

#include <ostream>
#include <string>
#include <vector>

void split(const std::string& str, char delimiter,
           std::vector<std::string>& result);

std::ostream& operator<<(std::ostream& out, const std::vector<int>& vec);
