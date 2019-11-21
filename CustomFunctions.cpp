#include "CustomFunctions.h"

using namespace std;

void split(const string &str, char delimiter, vector<string> &result)
{
	string::size_type i = 0;
	string::size_type j = str.find(delimiter);

	while (j != string::npos)
	{
		result.push_back(str.substr(i, j - i));
		i = ++j;
		j = str.find(delimiter, j);

		if (j == string::npos)
			result.push_back(str.substr(i, str.length()));
	}
}

ostream &
operator<<(ostream &out, const vector<int> &vec)
{
	std::vector<int>::const_iterator it;
	out << "(";
	for (it = vec.begin(); it != vec.end(); ++it)
	{
		out << *it << ", ";
	}
	out << "\b\b)";

	return out;
}