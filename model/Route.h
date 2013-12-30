#include <string>
#include <vector>
#include <exception>
using namespace std;

#ifndef __Route_h__
#define __Route_h__

#include "RouteSegment.h"
#include "RouteNode.h"

class RouteSegment;
class RouteNode;
class Route;

class Route
{
	public: std::vector<RouteSegment*> _edges;
	public: std::vector<RouteNode*> _nodes;
	public: Route(void);
	public: ~Route(void);
};

#endif
