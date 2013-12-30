#include <string>
#include <vector>
#include <exception>
using namespace std;

#ifndef __NavigableSpaceBoundary_h__
#define __NavigableSpaceBoundary_h__

#include "AbstractSpaceBoundary.h"

class AbstractSpaceBoundary;
class NavigableSpaceBoundary;

class NavigableSpaceBoundary: public AbstractSpaceBoundary
{
public: bool isNavigable(void);
};

#endif
