#include <string>
#include <vector>
#include <exception>
using namespace std;

#ifndef __Storey_h__
#define __Storey_h__

class Storey;

class Storey
{
public:
	int storeyNumber;
	std::string storeyName;
	double elevation;
};

#endif
