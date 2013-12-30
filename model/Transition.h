#include <string>
#include <vector>
#include <exception>
using namespace std;

#ifndef __Transition_h__
#define __Transition_h__

// #include "AbstractSpaceBoundary.h"
#include "State.h"

class AbstractSpaceBoundary;
class State;
class Transition;

class Transition
{
	public: static int idS;
			int id;
	public: AbstractSpaceBoundary* _unnamed_AbstractSpaceBoundary_;
	public: Segment_2 geometry;
	public: State* from;
	public:	State* to;
			Transition(void);
};

#endif
