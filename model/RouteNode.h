#include <string>
#include <vector>
#include <exception>
using namespace std;

#ifndef __RouteNode_h__
#define __RouteNode_h__

#include "State.h"

class State;
class RouteNode;

class RouteNode: public State
{
public:
	RouteNode(void);
};

#endif
