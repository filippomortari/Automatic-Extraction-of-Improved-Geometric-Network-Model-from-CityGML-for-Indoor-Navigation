#pragma once
#include "topologicalModel/TopologicalDataStructure.h"
#include "model/AbstractSpace.h"

typedef CGAL::Partition_traits_2<K>                         Traits;

class OgrExporter
{
public:
	OgrExporter(void);
	~OgrExporter(void);
	void toShapeFile(TopologicalDataStructure*);
	void vertexToShapeFile(TopologicalDataStructure * );
	void doorToShapeFile(TopologicalDataStructure *);
	void drawPolygonsModel(vector<AbstractSpace*>);
	static void printPoly(std::list<Point_2>, int );
	static void printPoly(Traits::Polygon_2 , int );
	static bool fileExists(const std::string& );
	static void printVertices(Polygon_2 , int );
	static void drawTriangulation(vector<list<Point_2> > , int ,vector<int>);
	static void drawTransitions(vector<Segment_2> , int);
};

