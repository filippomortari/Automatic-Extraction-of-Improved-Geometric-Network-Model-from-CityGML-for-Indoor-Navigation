#include "cityGMLParser.h"


cityGMLParser::cityGMLParser(Building* building){
	myBuilding = building;
	precisionRateForStoreys = 0.20;
	parameters = EnvironmentParameters::getInstance();
}

cityGMLParser::~cityGMLParser(void){
	for(int i=0; i<myRooms.size();++i){
		delete myRooms[i];
	}
	myRooms.clear();
}

void cityGMLParser::loadFile(char * fileName){
	
	int x =doc.LoadFile( fileName );
	XMLElement* buildingNode = doc.FirstChildElement("core:CityModel")->FirstChildElement("core:cityObjectMember")->FirstChildElement("bldg:Building");
	int count = 0;
	for(XMLElement* intRoom = buildingNode->FirstChildElement("bldg:interiorRoom"); intRoom != NULL; intRoom = intRoom->NextSiblingElement("bldg:interiorRoom")){
		XMLElement* currentRoom = intRoom->FirstChildElement("bldg:Room");
		extractRoom(currentRoom);
		std::sort(storeys.begin(),storeys.end());
		++count;
	}

	cout<<"number of rooms is "<<count<<endl;
} 

void cityGMLParser::extractRoom(XMLElement*& room){
	

	XMLElement* currentRoom = room;
	myRooms.push_back(new CityGMLRoom());
	CityGMLRoom* cityRoom = myRooms.back();
	cityRoom->id = currentRoom->Attribute("gml:id");
	XMLElement* geometryHandle = currentRoom->FirstChildElement("bldg:lod4Solid")->FirstChildElement("gml:Solid")->FirstChildElement("gml:exterior")->FirstChildElement("gml:CompositeSurface");
	//cout<<"ID-> "<<cityRoom->id<<endl;
	for(XMLElement* surface = geometryHandle->FirstChildElement("gml:surfaceMember"); surface != NULL; surface = surface->NextSiblingElement("gml:surfaceMember")){
		/*ok, I am cheating there:
		technically for each room, I should retrieve the geometry. but, the geometry in the file I am using
		is a reference to boundedBy tag (InteriorWall). so basically I retrieve it from there, without the annoyiance of
		doing a nested cycle. for this task a parser that implements XPath would have served better this need
		*/
		if(surface->Attribute("xlink:href")!=NULL){
			//the geometry is directly linked here, so need to retrieve it
			//cout<<"id of the room containing a xlink:href --->"<<cityRoom->id<<endl;
		}
		else{
			if(surface->FirstChildElement("gml:OrientableSurface")!=NULL){
				//cout<<surface->FirstChildElement("gml:OrientableSurface")->FirstChildElement("gml:baseSurface")->Attribute("xlink:href")<<endl;
			}
		}

	}
	double levelFloor;
	for(XMLElement* boundedBy = currentRoom->FirstChildElement("bldg:boundedBy"); boundedBy != NULL; boundedBy = boundedBy->NextSiblingElement("bldg:boundedBy")){
		if(boundedBy->FirstChildElement("bldg:InteriorWallSurface")!=NULL){
			//cout<<"----- muro"<<endl;
			for(XMLElement* doorHandle = boundedBy->FirstChildElement("bldg:InteriorWallSurface")->FirstChildElement("bldg:opening");
				doorHandle != NULL; doorHandle = doorHandle->NextSiblingElement("bldg:opening")){
					if(doorHandle->FirstChildElement("bldg:Door")!=NULL){
						//cout<<"---------- porta!"<<endl;
						list<Point_3> doorCoordinates;
						list<Point_2> doorCoordToBeStored;
						double minCoord;
						XMLElement* currDoor = doorHandle->FirstChildElement("bldg:Door")->FirstChildElement("bldg:lod4MultiSurface")->FirstChildElement("gml:MultiSurface")
							->FirstChildElement("gml:surfaceMember");
						if(currDoor->FirstChildElement("gml:Polygon")!=NULL){
							Polygon_2 tmPoly;
							doorCoordinates.clear();
							doorCoordToBeStored.clear();
							for(XMLElement* coord = currDoor->FirstChildElement("gml:Polygon")->FirstChildElement("gml:exterior")->FirstChildElement("gml:LinearRing")->FirstChildElement("gml:pos");
								coord!=NULL;coord=coord->NextSiblingElement("gml:pos")){
									string myCoords =coord->GetText();
									//cout<<myCoords<<endl;
									Point_3 currCoordinate = getCoordsFromString(myCoords);
									if(doorCoordinates.size()==0)
										minCoord = CGAL::to_double(currCoordinate.z());
									doorCoordinates.push_back(currCoordinate);
									if(CGAL::to_double(currCoordinate.z()) < minCoord)
										minCoord = CGAL::to_double(currCoordinate.z());
							}
						}
						else if (currDoor->FirstChildElement("gml:OrientableSurface")!=NULL){
						//hardcoded, should check if there was a geometry first, then the link
							string myLinkDoor = currDoor->FirstChildElement("gml:OrientableSurface")->FirstChildElement("gml:baseSurface")->Attribute("xlink:href");
							//cout<<myLinkDoor<<endl;
							XMLElement* ele = getElementByName(doc,"gml:id",myLinkDoor);
							if (ele!=NULL){
								doorCoordinates.clear();
								doorCoordToBeStored.clear();
								for(XMLElement* coordinates = ele->FirstChildElement("gml:exterior")->FirstChildElement("gml:LinearRing")->FirstChildElement("gml:pos");
									coordinates!=NULL; coordinates= coordinates->NextSiblingElement("gml:pos")){
									string myCoords =coordinates->GetText();
									//cout<<myCoords<<endl;
									Point_3 currCoordinate = getCoordsFromString(myCoords);
									if(doorCoordinates.size()==0)
										minCoord = CGAL::to_double(currCoordinate.z());
									doorCoordinates.push_back(currCoordinate);
									if(CGAL::to_double(currCoordinate.z()) < minCoord)
										minCoord = CGAL::to_double(currCoordinate.z());
								}
								//retrieving footprints of a door
								
							}
						}
						for (list<Point_3>::iterator it=doorCoordinates.begin(); it!=doorCoordinates.end();++it){
							//we want to retrieve the footprints of a door, just check for the lowest z coordinate		
							if (CGAL::to_double((*it).z())>minCoord)
										;
									else{
										bool found = false;
										Point_2 curPoint = Point_2((*it).x(),(*it).y());
										//avoid duplicate points
										for(list<Point_2>::iterator drIt = doorCoordToBeStored.begin();drIt!=doorCoordToBeStored.end() && !found;++drIt){
											if (curPoint == (*drIt))
												found = true;
										}
										if (!found)
											doorCoordToBeStored.push_back(curPoint);
									}
										
								}
								cityRoom->doors.push_back(doorCoordToBeStored);
						//->FirstChildElement("gml:Polygon")->Attribute("gml:id");
					}

			}

		}
		else if (boundedBy->FirstChildElement("bldg:FloorSurface")!=NULL){
			//cout<<"----- pavimento"<<endl;
			XMLElement* coordinates = boundedBy->FirstChildElement("bldg:FloorSurface")->FirstChildElement("bldg:lod4MultiSurface")
				->FirstChildElement("gml:MultiSurface")->FirstChildElement("gml:surfaceMember")->FirstChildElement("gml:Polygon")
				->FirstChildElement("gml:exterior")->FirstChildElement("gml:LinearRing");
			for(XMLElement* coord = coordinates->FirstChildElement("gml:pos");coord != NULL; coord = coord->NextSiblingElement("gml:pos")){
				string myCoords =coord->GetText();
				Point_3 currCoordinate = getCoordsFromString(myCoords);
				//last point is == to the first one, don't wanna add it
				if(coord->NextSiblingElement()!=NULL){
					cityRoom->roomSurfaces.push_back(Point_2(currCoordinate.x(),currCoordinate.y()));
					//this will serve to the construction of floor concept
					levelFloor = CGAL::to_double(currCoordinate.z());
					
				}
				
			}

		}
		//pay attention to staircase: suggestion, if closuresurface.z coordinate == 
		else if (boundedBy->FirstChildElement("bldg:ClosureSurface")!=NULL){
			//cout<<"----- closure"<<endl;
		}
	}
	
	bool foundFloor = false;
	if (storeys.size()==0){
		storeys.push_back(levelFloor);
		//roomsInStoreys is a map holding rooms with same floor level,
		//storeys is an array of doubles, which is sorted: floor 0, floor1, etc...
		roomsInStoreys[levelFloor].push_back(cityRoom);
		cityRoom->roomLevel = levelFloor;
	}
	else{
		for(vector<double>::iterator dblIt = storeys.begin(); dblIt!=storeys.end() && !foundFloor; ++dblIt){
			double currValue = (*dblIt);
			if((levelFloor> currValue-precisionRateForStoreys) && (levelFloor < currValue + precisionRateForStoreys)){
				foundFloor = true;
				roomsInStoreys[currValue].push_back(cityRoom);
				cityRoom->roomLevel = currValue;
			}
		}
		//here I build the concept of storey
		if(!foundFloor){
			storeys.push_back(levelFloor);
			roomsInStoreys[levelFloor].push_back(cityRoom);
			cityRoom->roomLevel = levelFloor;
		}
	}
}

Point_3 cityGMLParser::getCoordsFromString(string s){
	std::istringstream iss(s);
	double value, x,y,z;
	int cnt = 0;
	while ( iss >> value ){	
		++cnt;
		if(cnt==1)
			x=value;
		else if (cnt ==2)
			y=value;
		else if (cnt ==3)
			z=value;
	}
	return Point_3(x,y,z);
}

XMLElement * cityGMLParser::getElementByName(XMLDocument & doc, std::string const & elemt_attr_name,  std::string const & elemt_value) {

	XMLElement * elem = doc.RootElement(); //Tree root
	while (elem) {
		if(elem->Attribute(elemt_attr_name.c_str(),elemt_value.c_str()) != NULL)
			return (elem);
		/*if (!std::string(elem->Value()).compare(elemt_value)) 
			return (elem);*/
		/*elem = elem->NextSiblingElement();*/
		if (elem -> FirstChildElement()) {
			elem = elem -> FirstChildElement();
		} else if (elem -> NextSiblingElement()) {
			elem = elem -> NextSiblingElement();
		} else {
			while (!elem ->NextSiblingElement()) {
				if (elem -> Parent() -> ToElement() == doc.RootElement()) {
					return NULL;
				}
				elem = elem -> Parent() -> ToElement();
			}
			elem = elem ->NextSiblingElement();
		}
	}
	return (NULL);
}

void cityGMLParser::populateModel(void){
	for(vector<double>::iterator it=storeys.begin(); it!=storeys.end(); ++it){
		myBuilding->addStorey(it - storeys.begin(),"",(*it));
		
	}
	
	//create spaces to be fetched into the data model
	NavigableSpace* mySpace = NULL;
	NavigableSpace* myDoorSpace = NULL;
	int count = 0;
	for(vector<CityGMLRoom*>::iterator rmIt=myRooms.begin(); rmIt!=myRooms.end();++rmIt){
		if((*rmIt)->doors.size()==1){
			mySpace = (NavigableSpace*) myBuilding->addGeneralSpace();
			mySpace->spaceType = GENERALSPACE;
			--count;
		}
		else if((*rmIt)->doors.size()>1){
			mySpace = (NavigableSpace*) myBuilding->addTransitionSpace();
			mySpace->spaceType = TRANSITIONSPACE;
			--count;
		}
		else
			mySpace = NULL;

		if(mySpace != NULL){
			mySpace->id = count;
			mySpace->cityID = (*rmIt)->id;
			mySpace->geometry = (*rmIt)->roomSurfaces;
			//storey information
			Storey* myStorey;
			for(vector<Storey>::iterator storey=myBuilding->buildingStoreys.begin(); storey!= myBuilding->buildingStoreys.end(); ++storey){
				
				if((*storey).elevation == (*rmIt)->roomLevel){
					myStorey = &(*storey);
					mySpace->_belongingStorey.push_back(myStorey);
				}
				
			}
			
			for(EdgeIterator edgIt=mySpace->geometry.edges_begin(); edgIt!=mySpace->geometry.edges_end();++edgIt){
				bool isDoor = false;
				for(vector<list<Point_2> >::iterator drIt= (*rmIt)->doors.begin();drIt!=(*rmIt)->doors.end() && !isDoor;++drIt){
					Segment_2 doorSeg = Segment_2((*drIt).front(),(*drIt).back());
					//navigable space boundary
					if(approxIntersection((*edgIt),doorSeg)){
						isDoor = true;
						//cout<<"door is found for room "<<mySpace->cityID<<endl;
						--count;
						myDoorSpace = (NavigableSpace*) myBuilding->addDoorSpace();
						myDoorSpace->spaceType = DOORSPACE;
						myDoorSpace->id=count;
						myDoorSpace->_belongingStorey.push_back(myStorey);
						//geometry of the door
						myDoorSpace->geometry.push_back(doorSeg.source());
						myDoorSpace->geometry.push_back(doorSeg.target());
						//boundary of the door
						myDoorSpace->_boundedBy.push_back(new NavigableSpaceBoundary());
						myDoorSpace->_boundedBy.back()->geometry.push_back(doorSeg.source());
						myDoorSpace->_boundedBy.back()->geometry.push_back(doorSeg.target());

						mySpace->_boundedBy.push_back(new NavigableSpaceBoundary());
						mySpace->_boundedBy.back()->geometry.push_back((*edgIt).source());
						mySpace->_boundedBy.back()->geometry.push_back((*edgIt).target());
						//add door as a neighbour to room and room as a neighbour to door
						mySpace->addNeighbour(myDoorSpace);
						myDoorSpace->addNeighbour(mySpace);
					}
				}
				//we have to add an abstract space boundary, not navigable
				if(isDoor==false){
					mySpace->_boundedBy.push_back(new AbstractSpaceBoundary());
					mySpace->_boundedBy.back()->geometry.push_back((*edgIt).source());
					mySpace->_boundedBy.back()->geometry.push_back((*edgIt).target());
				}
			}
			//cout<<"mySpace has neighbours: "<<mySpace->_neighbours.size()<<endl;
			//cout<<"number of doors was: "<<(*rmIt)->doors.size()<<endl<<endl;
			//cout<<"number of space boundaries: "<<mySpace->_boundedBy.size()<<endl<<endl;
		}
	}

	/*instead of removing door duplicates (one door for each room, so 2 rooms sharing the same door have 2 different instances)
	/*link the door duplicates one each other, in order to mantain connectivity
	*/
	map<int,bool> doorDuplicate;
	for(vector<AbstractSpace*>::iterator spIt=myBuilding->buildingParts.begin();spIt!=myBuilding->buildingParts.end();++spIt){
		for(vector<AbstractSpace*>::iterator spItNest =myBuilding->buildingParts.begin();spItNest!=myBuilding->buildingParts.end();++spItNest){
			if((*spIt)->spaceType == DOORSPACE && (*spItNest)->spaceType == DOORSPACE && spIt!=spItNest){
				Segment_2 edge1, edge2;
				edge1 = (*spIt)->geometry.edge(0);
				edge2 = (*spItNest)->geometry.edge(0);
				if(approxIntersection(edge1,edge2)){
					/*cout<<"adjacent doors!"<<endl;
					NavigableSpace* nav = dynamic_cast <NavigableSpace*> (*spIt);
					NavigableSpace* nav2 = dynamic_cast <NavigableSpace*> (*spItNest);
					cout<<"door A neigh: "<<nav->_neighbours.front()->cityID<<endl;
					cout<<"door B neigh: "<<nav2->_neighbours.front()->cityID<<endl<<endl;
					*/
					NavigableSpace* nav = dynamic_cast <NavigableSpace*> (*spIt);
					NavigableSpace* nav2 = dynamic_cast <NavigableSpace*> (*spItNest);
					if(nav!=NULL && nav2!=NULL){
						nav->addNeighbour(nav2);
					}
				}
			}
		}
	}
	
}

bool cityGMLParser::approxIntersection(Segment_2 s1, Segment_2 s2){
	bool result = false;
	double precision = parameters->precision;
	Point_2 s1Start, s1End, s2Start, s2End;
	s1Start = s1.source();
	s1End = s1.target();
	s2Start = s2.source();
	s2End = s2.target();
	if(sqrt(CGAL::to_double (CGAL::squared_distance(s1Start,s2Start)))<precision){
		if(sqrt(CGAL::to_double (CGAL::squared_distance(s1End,s2End)))<precision)
			result = true;
	}

	else if (sqrt(CGAL::to_double (CGAL::squared_distance(s1Start,s2End)))<precision){
		if (sqrt(CGAL::to_double (CGAL::squared_distance(s1End,s2Start)))<precision)
			result = true;
	}
	return result;
}