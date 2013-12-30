#pragma once
#include <cstdlib>
#include <map>
#include "node.h"
#include "Edge.h"
#include "Area.h"
#include "../CGALDefines.h"
using namespace std;


class TopologicalDataStructure
{
public:
	map<int,node> nodes;
	map<int,Area> areas;
	map<int,Edge> edges;
	map<int,list<Point_3> > extrudedDoors;
	map<int,list<Point_3> > extrudedEdges;
	
	TopologicalDataStructure(void);
	~TopologicalDataStructure(void);
	void addNode(node);
	void addEdge(Edge);
	void addArea(Area);
	map<int,Edge> getEdges(void);
};

