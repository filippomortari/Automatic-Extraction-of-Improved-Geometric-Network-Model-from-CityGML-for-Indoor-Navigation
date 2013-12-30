#include <string>
#include <vector>
#include <exception>
using namespace std;

#ifndef __NonNavigableSpace_h__
#define __NonNavigableSpace_h__

// #include "NavigableSpace.h"
#include "AbstractSpace.h"

class NavigableSpace;
class AbstractSpace;
class NonNavigableSpace;

class NonNavigableSpace: public AbstractSpace
{
	public: NavigableSpace* _unnamed_NavigableSpace_;
};

#endif
