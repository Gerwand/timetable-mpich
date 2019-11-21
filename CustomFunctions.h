#pragma once

#include <vector>
#include <string>
#include <ostream>

void split(const std::string &str, char delimiter,
		   std::vector<std::string> &result);

std::ostream &operator<<(std::ostream &out, const std::vector<int> &vec);
