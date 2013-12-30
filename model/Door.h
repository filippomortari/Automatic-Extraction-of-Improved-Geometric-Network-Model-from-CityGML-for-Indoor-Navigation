#include <string>
#include <vector>
#include <exception>
using namespace std;

#ifndef __Door_h__
#define __Door_h__

#include "ConnectedSpace.h"

class ConnectedSpace;
class Door;

class Door: public ConnectedSpace
{
	void foo(void);
};

#endif
