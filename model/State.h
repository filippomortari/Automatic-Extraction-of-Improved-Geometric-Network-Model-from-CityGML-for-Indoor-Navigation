#include <string>
#include <vector>
#include <exception>
#include "../CGALDefines.h"
using namespace std;

#ifndef __State_h__
#define __State_h__

#include "Transition.h"
// #include "AbstractSpace.h"

class Transition;
class AbstractSpace;
class State;

class State
{
public: 
	static int idS;
	int id;
	std::vector<Transition*> transitions;
	AbstractSpace* representedSpace;
	Point_2 geometry;
	bool isForNavigation;
	State(void);
};

#endif
