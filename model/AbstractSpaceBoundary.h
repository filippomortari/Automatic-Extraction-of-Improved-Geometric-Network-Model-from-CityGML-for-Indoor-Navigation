#include <string>
#include <vector>
#include <list>
#include <exception>
#include "../CGALDefines.h"

using namespace std;

#ifndef __AbstractSpaceBoundary_h__
#define __AbstractSpaceBoundary_h__

// #include "Transition.h"

class Transition;
class AbstractSpaceBoundary;

class AbstractSpaceBoundary
{
public: 
	Transition* _duality;
	std::list<Point_2> geometry;
	virtual bool isNavigable(void);
	std::list<Point_2> getGeometry(void);
};

#endif
