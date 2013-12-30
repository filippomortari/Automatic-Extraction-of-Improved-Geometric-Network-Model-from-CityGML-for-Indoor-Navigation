#pragma once
#ifndef NETWORKGENERATOR_H
#define NETWORKGENERATOR_H
#include "../CGALDefines.h"
#include "../model/Building.h"
#include "../model/Route.h"
#include "../model/NavigableSpace.h"
#include "../model/VirtualNavigableBoundary.h"
#include "../control/EnvironmentParameters.h"
#include "../OgrExporter.h"
#include <boost/shared_ptr.hpp>
#include <algorithm>
#include <iostream>
#include <fstream>
using namespace std;

typedef CGAL::Exact_predicates_exact_constructions_kernel K ;

typedef K::Point_2                    Point_2 ;
typedef CGAL::Polygon_2<K>            Polygon_2 ;
typedef K::FT                        FT ;
typedef CGAL::Straight_skeleton_2<K> Ss ;

typedef boost::shared_ptr<Polygon_2> PolygonPtr ;
typedef boost::shared_ptr<Ss> SsPtr ;

typedef std::vector<PolygonPtr> PolygonPtrVector ;





class NetworkGenerator {
private:
	vector <Polygon_2> computeBoundarySampling(vector<Polygon_2>,double, NavigableSpace*,double);
	bool checkIfEdgeisRoomNavigableBoundary(Segment_2, NavigableSpace*, NavigableSpace*& );
	void markDomains(CDT&);
	void markDomains(CDT& , CDT::Face_handle, int, std::list<CDT::Edge>&);
	bool approxIntersection(Segment_2, Segment_2 );
	void placeNodesInTriangles(CDT&);
	void linkNodes(Route*, CDT&);
	void studyHolesForAdditionalNodes(vector<Polygon_2>,CDT&, NavigableSpace*);
public:
	Building* myBuild;
	Route* myRoute;
	EnvironmentParameters* parameters;
	map<int,list<CDT::Face_handle> > facesWithSemantics; //for a faster access
	list<NavigableSpace*> crossingSpaces; //for a faster access
	NetworkGenerator(Building *, Route *);
	~NetworkGenerator(void);
	void generateNetwork(void);
	void computeConstraineDelaunay(vector<Polygon_2>, CDT&);
	void insertPolygonIntoCDT(CDT& ,const Polygon_2& );
	void exportRoute(int);
};

#endif