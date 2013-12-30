#include <string>
#include <vector>
#include <exception>
#include <list>
using namespace std;

#ifndef __AbstractSpace_h__
#define __AbstractSpace_h__

// #include "State.h"

#include "../config.h"
#include "../CGALDefines.h"
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>

using namespace std;


class State;
class AbstractSpaceBoundary;
class Storey;
class AbstractSpace;


class AbstractSpace
{
public: 
	State* relatedState;
	int id;
	string cityID;
	std::vector<AbstractSpaceBoundary*> _boundedBy;
	std::vector<Storey*> _belongingStorey; 
	Polygon_2 geometry;
	SpaceSemantics spaceType;

	AbstractSpace(void);
	~AbstractSpace(void);
	virtual void foo(void) = 0;
	
};

#endif
