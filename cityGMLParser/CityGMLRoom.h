#pragma once
#include <string>
#include "../CGALDefines.h"
#include "../print.h"
#include <vector>
#include <list>

class CityGMLRoom{
private:
public:
	std::string id;
	double roomLevel;
	Polygon_2 roomSurfaces;
	std::vector<std::list<Point_2> > doors;
};