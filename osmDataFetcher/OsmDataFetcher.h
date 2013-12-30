#pragma once
#include "pugixml.hpp"

#include "../topologicalModel/TopologicalDataStructure.h"
#include "../topologicalModel/node.h"
#include "../topologicalModel/TopologicalDataStructure.h"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/squared_distance_3.h>
#include <CGAL/Polygon_2.h>
#include "../config.h"
#include <map>
#include <list>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

//model
#include "../model/Building.h"
#include "../model/AbstractSpace.h"
#include "../model/GeneralSpace.h"
#include "../model/NavigableSpaceBoundary.h"

using namespace std;

class OsmDataFetcher
{

private:
	
public:
	TopologicalDataStructure * myTopologicalDataStructure;
	OsmDataFetcher(TopologicalDataStructure *);
	~OsmDataFetcher(void);

	void parseXML(void);
	void extrude(double);
	void extrudeDoors(double);
	double slopeSegmentAsAngle(double,double,double,double,int);
	void populateModel(Building*);

};

