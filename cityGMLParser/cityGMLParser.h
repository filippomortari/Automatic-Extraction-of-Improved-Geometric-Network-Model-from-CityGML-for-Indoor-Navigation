#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include "tinyxml2.h"
#include "CityGMLRoom.h"
#include "../model/Building.h"
#include "../control/EnvironmentParameters.h"

using namespace std;
using namespace tinyxml2;

class cityGMLParser{
private:
	void extractRoom(XMLElement*&);
	Point_3 getCoordsFromString(string );
	XMLElement * getElementByName(XMLDocument &, std::string const & ,std::string const &);
	bool approxIntersection(Segment_2, Segment_2 );
public:
	EnvironmentParameters* parameters;
	tinyxml2::XMLDocument doc;
	vector <CityGMLRoom*> myRooms;
	Building* myBuilding;
	/*
	storey information
	*/
	vector<double> storeys;
	double precisionRateForStoreys;
	map<double,list<CityGMLRoom*> > roomsInStoreys;

	cityGMLParser(Building*);
	~cityGMLParser(void);
	void loadFile(char* );
	void populateModel(void);
};