#include "OsmDataFetcher.h"
#include "../CGALDefines.h"


OsmDataFetcher::OsmDataFetcher(TopologicalDataStructure * t)
{
	myTopologicalDataStructure = t;
}


OsmDataFetcher::~OsmDataFetcher(void)
{
}

//reads the xml file and fetches its data into topologicalModel classes
void OsmDataFetcher::parseXML(void) {
	pugi::xml_document osmFile;
	pugi::xml_parse_result result = osmFile.load("xgconsole.xml");
	//std::cout << "Load result: " << result.description() << " attr value: [" << osmFile.first_child().value() << "]\n\n"<< std::endl;

	pugi::xml_document doc;
	if (!doc.load_file("/Users/filippomortari/Desktop/msc_thesis_application/msc_thesis_application/ground_floor.xml")) std::cout<<"error.. xml file not loaded";

	pugi::xml_node tools = doc.child("osm");

	//[code_traverse_iter
	std::string valueName, doorName;
	int id, count, stNode, endNode, edgeID;
	double x,y,z;
	bool isDoor;
	pugi::xml_node walker;
	node startN, endN;

	list <int> availableEdges;



	// key is the id of starting Node, the value list is the list containing the IDs of the edges incident to the node
	map <int, list<int> > tempEdges;
	map<int,list<int> >::const_iterator edgeIt;
	edgeID = 0;

	for (pugi::xml_node_iterator it = tools.begin(); it != tools.end(); ++it)
	{
		valueName = it->name();
		if (!Config::debug)
			std::cout << "Node type:" << valueName;


		//nodes parsing
		if (valueName=="node"){

			valueName = it->attribute("id").value();
			id =  it->attribute("id").as_int();
			x = it->attribute("lon").as_double();
			y = it->attribute("lat").as_double();
			z = 0;

			if (!Config::debug)
				std::cout <<" id: "<<id<<" lat: "<<x<<" lon: "<<y;



			isDoor = false;
			for (pugi::xml_node walker = it->child("tag"); walker; walker = walker.next_sibling("tag")){

				for (pugi::xml_attribute_iterator ait = walker.attributes_begin(); ait != walker.attributes_end(); ait++)
				{
					valueName = ait->name();
					if(valueName == "k"){
						valueName = ait->value();
						if(valueName == "door"){
							isDoor= true;

						}
						else if (valueName =="name"){
							doorName = ait->next_attribute().as_string();
							if (!Config::debug)
								std::cout<<" door = "<<isDoor<<" "<<doorName<<endl;
						}

					}

				}

			}

			//nodes added to the topological data structure 
			node tempNode;
			tempNode.setId(id);
			tempNode.setGeometry(Point_3(x,y,z));
			tempNode.setIsDoor(isDoor);
			if(isDoor)
				tempNode.setDoorName(doorName);
			myTopologicalDataStructure->addNode(tempNode);

		}//end nodes

		else if (valueName == "way"){
			valueName = it->attribute("id").value();
			id =  it->attribute("id").as_int();
			if (!Config::debug)
				std::cout <<"way id: "<<id<<endl;
			Area tempArea;
			tempArea.setId(id);
			myTopologicalDataStructure->addArea(tempArea);

			count = 0;
			for (pugi::xml_node way = it->child("nd"); way; way = way.next_sibling("nd")){
				//i am scanning the first node
				if(count ==0){
					stNode = way.attribute("ref").as_int();
					endNode = way.attribute("ref").as_int();
				}
				// scanning each edge
				else {
					stNode = endNode;
					endNode = way.attribute("ref").as_int();
				}

				//fetching edges into an efficient data structure
				if(stNode != endNode) {


					Edge tempEdg;
					//i am creating here the unique ID of an edge, then populate edge
					++edgeID;
					tempEdg.setID(edgeID);
					myTopologicalDataStructure->addEdge(tempEdg);
					tempEdg = myTopologicalDataStructure->edges[edgeID];

					startN = myTopologicalDataStructure->nodes[stNode];
					endN = myTopologicalDataStructure->nodes[endNode];

					myTopologicalDataStructure->edges[edgeID].setStartNode(&myTopologicalDataStructure->nodes[stNode]);
					myTopologicalDataStructure->edges[edgeID].setEndNode( &myTopologicalDataStructure->nodes[endNode]);

					if((startN.isDoor && endN.isDoor)&&(startN.doorName==endN.doorName))
						myTopologicalDataStructure->edges[edgeID].semantics = DOOR;
					else
						myTopologicalDataStructure->edges[edgeID].semantics = WALL;
					//nodes are stored always in CW order, my assumption
					myTopologicalDataStructure->edges[edgeID].setRightArea(&myTopologicalDataStructure->areas[id]);
					//populate also lookup table, for fast retrieval
					tempEdges[stNode].push_back(edgeID);

					myTopologicalDataStructure->areas[id].addEdge(&myTopologicalDataStructure->edges[edgeID]);



					// see if we can fetch also left area info
					edgeIt = tempEdges.find(endNode);
					if (edgeIt!=tempEdges.end()){
						availableEdges = tempEdges[endNode];
						int k;
						for(std::list<int>::iterator myIt = availableEdges.begin(); myIt != availableEdges.end();myIt++){
							k = (*myIt);
							if(myTopologicalDataStructure->edges[k].endNode->id == stNode){
								myTopologicalDataStructure->edges[k].leftArea = &myTopologicalDataStructure->areas[id];
								myTopologicalDataStructure->edges[edgeID].leftArea = myTopologicalDataStructure->edges[k].rightArea;
								myTopologicalDataStructure->edges[k].oppositeEdge = &myTopologicalDataStructure->edges[edgeID];
								myTopologicalDataStructure->edges[edgeID].oppositeEdge= &myTopologicalDataStructure->edges[k];

							}
						}
					}



				}

				if (!Config::debug)
					std::cout << "nd ref: " << way.attribute("ref").as_int();
				count++;

			}

			if (Config::debug)
				std::cout <<endl;

		}
	}
}

void OsmDataFetcher::extrude(double precision){
	Point_3 upperLeft, upperRight, lowerLeft, lowerRight;
	double x1,x2,x3,y1,y2,y3,z, deltaX, deltaY, angle, angleOffset, r;
	map<int,bool> isVisited;

	for (map<int,Edge>::iterator it1=myTopologicalDataStructure->edges.begin(); it1!=myTopologicalDataStructure->edges.end();++it1){
		isVisited[(*it1).second.getID()] = false;
	}
	for (map<int,Edge>::iterator it=myTopologicalDataStructure->edges.begin(); it!=myTopologicalDataStructure->edges.end();++it){


		if (isVisited[(*it).second.getID()] == false)
		{
			isVisited[(*it).second.getID()] = true;
			if((*it).second.oppositeEdge != NULL)
				isVisited[(*it).second.oppositeEdge->getID()] = true;
			Point_3 startP = (*it).second.getStartNode()->getGeometry();
			Point_3 endP = (*it).second.getEndNode()->getGeometry();



			x1= CGAL::to_double(startP.x());
			x2= CGAL::to_double(endP.x());
			y1= CGAL::to_double(startP.y());
			y2= CGAL::to_double(endP.y());
			z= CGAL::to_double(startP.z());
			angle = slopeSegmentAsAngle(x1,x2,y1,y2,(*it).second.getID());
			if (x2-x1 < 0)
				angle = angle + M_PI;
			//cout<<"angle for edge "<<(*it).second.getID()<<" = "<<angle*180 / M_PI<<" "<<endl;
			r = precision*(sqrt(2.0));

			//upperLeft
			angleOffset = angle + M_PI_4;
			deltaX = r*cos(angleOffset);
			deltaY = r*sin(angleOffset);
			x3 = x2 + deltaX;
			y3 = y2 + deltaY;
			upperLeft = Point_3(x3,y3,z);

			//upperRight
			angleOffset = angle - M_PI_4;
			deltaX = r*cos(angleOffset);
			deltaY = r*sin(angleOffset);
			x3 = x2 + deltaX;
			y3 = y2 + deltaY;
			upperRight = Point_3(x3,y3,z);

			//lowerLeft
			angleOffset = angle + M_PI_4 + M_PI_2;
			deltaX = r*cos(angleOffset);
			deltaY = r*sin(angleOffset);
			x3 = x1 + deltaX;
			y3 = y1 + deltaY;
			lowerLeft = Point_3(x3,y3,z);

			//lowerRight
			angleOffset = angle + M_PI + M_PI_4;
			deltaX = r*cos(angleOffset);
			deltaY = r*sin(angleOffset);
			x3 = x1 + deltaX;
			y3 = y1 + deltaY;
			lowerRight = Point_3(x3,y3,z);

			//create the polygon always in CW order, STARTING from lowerRight vertex
			myTopologicalDataStructure->extrudedEdges[(*it).second.getID()].push_back(lowerRight);
			myTopologicalDataStructure->extrudedEdges[(*it).second.getID()].push_back(lowerLeft);
			myTopologicalDataStructure->extrudedEdges[(*it).second.getID()].push_back(upperLeft);
			myTopologicalDataStructure->extrudedEdges[(*it).second.getID()].push_back(upperRight);

		} 
	}
	//cout<<CGAL::squared_distance(myTopologicalDataStructure->nodes[-368].geometry,myTopologicalDataStructure->nodes[-54].geometry)<<endl;
	/*cout<<"distaance: "<<y2-y1<<endl;		


	# starting point
	x0 = 1.0
	y0 = 1.5

	# theta is the angle (in radians) of the direction in which to move
	theta = pi/6

	# r is the distance to move
	r = 2.0

	deltax = r*cos(theta)
	deltay = r*sin(theta)

	# new point
	x1 = x0 + deltax
	y1 = y0 + deltay
	*/
}



double OsmDataFetcher::slopeSegmentAsAngle(double a, double b, double c, double d,int i){
	double result;
	if ((b-a) == 0 && (d-c) > 0){
		result = M_PI_2;

	}

	else if ((b-a) == 0 && (d-c) < 0){
		result = - M_PI_2;

	}

	else result = atan((d-c)/(b-a));

	return result;
}

void OsmDataFetcher::extrudeDoors(double precision){
	Point_3 upperLeft, upperRight, lowerLeft, lowerRight;
	double x1,x2,x3,y1,y2,y3,z, deltaX, deltaY, angle, angleOffset, r;
	map<int,bool> isVisited;

	for (map<int,Edge>::iterator it1=myTopologicalDataStructure->edges.begin(); it1!=myTopologicalDataStructure->edges.end();++it1){
		isVisited[(*it1).second.getID()] = false;
	}
	for (map<int,Edge>::iterator it=myTopologicalDataStructure->edges.begin(); it!=myTopologicalDataStructure->edges.end();++it){


		if (isVisited[(*it).second.getID()] == false && (*it).second.semantics == DOOR )
		{	//cout<<"door Name"<<" "<<(*it).second.getStartNode()->doorName<<endl;
			isVisited[(*it).second.getID()] = true;
			if((*it).second.oppositeEdge != NULL)
				isVisited[(*it).second.oppositeEdge->getID()] = true;
			Point_3 startP = (*it).second.getStartNode()->getGeometry();
			Point_3 endP = (*it).second.getEndNode()->getGeometry();



			x1= CGAL::to_double(startP.x());
			x2= CGAL::to_double(endP.x());
			y1= CGAL::to_double(startP.y());
			y2= CGAL::to_double(endP.y());
			z= CGAL::to_double(startP.z());
			angle = slopeSegmentAsAngle(x1,x2,y1,y2,(*it).second.getID());
			if (x2-x1 < 0)
				angle = angle + M_PI;
			//cout<<"angle for edge "<<(*it).second.getID()<<" = "<<angle*180 / M_PI<<" "<<endl;
			r = precision;

			//upperLeft
			angleOffset = angle + M_PI_2;
			deltaX = r*cos(angleOffset);
			deltaY = r*sin(angleOffset);
			x3 = x2 + deltaX;
			y3 = y2 + deltaY;
			upperLeft = Point_3(x3,y3,z);

			//upperRight
			angleOffset = angle - M_PI_2;
			deltaX = r*cos(angleOffset);
			deltaY = r*sin(angleOffset);
			x3 = x2 + deltaX;
			y3 = y2 + deltaY;
			upperRight = Point_3(x3,y3,z);

			//lowerLeft
			angleOffset = angle + M_PI_2;
			deltaX = r*cos(angleOffset);
			deltaY = r*sin(angleOffset);
			x3 = x1 + deltaX;
			y3 = y1 + deltaY;
			lowerLeft = Point_3(x3,y3,z);

			//lowerRight
			angleOffset = angle - M_PI_2;
			deltaX = r*cos(angleOffset);
			deltaY = r*sin(angleOffset);
			x3 = x1 + deltaX;
			y3 = y1 + deltaY;
			lowerRight = Point_3(x3,y3,z);

			//create the polygon always in CW order, STARTING from lowerRight vertex
			myTopologicalDataStructure->extrudedDoors[(*it).second.getID()].push_back(lowerRight);
			myTopologicalDataStructure->extrudedDoors[(*it).second.getID()].push_back(lowerLeft);
			myTopologicalDataStructure->extrudedDoors[(*it).second.getID()].push_back(upperLeft);
			myTopologicalDataStructure->extrudedDoors[(*it).second.getID()].push_back(upperRight);


		} 
	}

}

void OsmDataFetcher::populateModel(Building* building){

	//rooms
	int numDoors;
	Area tmpArea;
	Edge* tmpEdge;
	Building* mybuilding = building;
	NavigableSpace* mySpace = NULL;
	NavigableSpace* myDoorSpace = NULL;
	mybuilding->addStorey(0,"ground floor",0.0);
	int doorNumbers = 0;
	for(map<int,Area>::iterator it = myTopologicalDataStructure->areas.begin(); it != myTopologicalDataStructure->areas.end(); ++it){
		numDoors = 0;
		tmpArea = (*it).second;
		std::list<Point_2> points;

		for (list<Edge*>::iterator edgIt = tmpArea.edges.begin(); edgIt != tmpArea.edges.end(); ++edgIt){
			tmpEdge = (*edgIt);
			//check if there are any connectors
			if (tmpEdge->semantics == DOOR){
				numDoors++;
			}
			//here we construct the geometry component of abstract space	
			//points.push_back(Point_2(tmpEdge->getStartNode()->geometry.x(),tmpEdge->getStartNode()->geometry.y()));
		}
		//general space
		if (numDoors == 1 && tmpArea.id!= -492 && tmpArea.id!=-412){
			mySpace = (NavigableSpace*) mybuilding->addGeneralSpace();
			mySpace->spaceType = GENERALSPACE;
		}

		//transition space
		else if (numDoors>1 && tmpArea.id!= -492 && tmpArea.id!=-412){
			mySpace = (NavigableSpace*) mybuilding->addTransitionSpace();
			mySpace->spaceType = TRANSITIONSPACE;
		}

		else if (tmpArea.id == -492) {
			mySpace = (NavigableSpace*) mybuilding->addVerticalTransitionSpace(ELEVATORSPACE);
			mySpace->spaceType = ELEVATORSPACE;
		}

		else if (tmpArea.id == -412) {
			mySpace = (NavigableSpace*) mybuilding->addVerticalTransitionSpace(STAIRSPACE);
			mySpace->spaceType = STAIRSPACE;
		}

		//the building boundary, delete it from model, unnecessary
		else if (tmpArea.id == -450) {
			mySpace = NULL;
		}

		else if (numDoors == 0) {
			mySpace = NULL;
		}


		if (mySpace != NULL){
			mySpace->id = tmpArea.id;
			mySpace->_belongingStorey.push_back(&mybuilding->buildingStoreys.back());
			for (list<Edge*>::iterator edgIt = tmpArea.edges.begin(); edgIt != tmpArea.edges.end(); ++edgIt){
				tmpEdge = (*edgIt);
				//create the geometry of the navigable space
				mySpace->geometry.push_back(Point_2(tmpEdge->getStartNode()->geometry.x(),tmpEdge->getStartNode()->geometry.y()));

				//check if there are any connectors
				if (tmpEdge->semantics == DOOR ){
					
					
					mySpace->_boundedBy.push_back(new NavigableSpaceBoundary());
					//populate the boundaries, no matter if they are navigable or nonnavigable
					mySpace->_boundedBy.back()->geometry.push_back(Point_2(tmpEdge->getStartNode()->geometry.x(),tmpEdge->getStartNode()->geometry.y()));
					mySpace->_boundedBy.back()->geometry.push_back(Point_2(tmpEdge->getEndNode()->geometry.x(),tmpEdge->getEndNode()->geometry.y()));
					
					//consider door only once
					map <int,list<Point_3> >::iterator pointIt = myTopologicalDataStructure->extrudedDoors.find(tmpEdge->getID());
					
					if (pointIt!=myTopologicalDataStructure->extrudedDoors.end()){	
						
						//create a door as a space
						myDoorSpace = (NavigableSpace*) mybuilding->addDoorSpace();
						myDoorSpace->spaceType = DOORSPACE;
						myDoorSpace->id=tmpEdge->getID();
						myDoorSpace->_belongingStorey.push_back(&mybuilding->buildingStoreys.back());
						//retrieve the geometry of the previously extruded door
						


						map <int,list<Point_3> >::iterator pointIt = myTopologicalDataStructure->extrudedDoors.find(myDoorSpace->id);
						if (pointIt!=myTopologicalDataStructure->extrudedDoors.end()){
							doorNumbers++;
							for (list<Point_3>::const_iterator doorIt= myTopologicalDataStructure->extrudedDoors[myDoorSpace->id].begin(); doorIt!=myTopologicalDataStructure->extrudedDoors[myDoorSpace->id].end(); ++doorIt){
								double x, y;
								x = CGAL::to_double((*doorIt).x());
								y = CGAL::to_double((*doorIt).y());
								
								myDoorSpace->geometry.push_back(Point_2(x,y));
							}
						}

						else{
						}

						//I am cheating here, i put the edge segment, not the actual boundaries of the door...
						myDoorSpace->_boundedBy.push_back(new NavigableSpaceBoundary());
						myDoorSpace->_boundedBy.back()->geometry.push_back(Point_2(tmpEdge->getStartNode()->geometry.x(),tmpEdge->getStartNode()->geometry.y()));
						myDoorSpace->_boundedBy.back()->geometry.push_back(Point_2(tmpEdge->getEndNode()->geometry.x(),tmpEdge->getEndNode()->geometry.y()));

					}

					//cout<<"extruded doors: "<<myTopologicalDataStructure->extrudedDoors.size()<<". Number of door spaces: "<<doorNumbers<<endl;
				} // end Door analysis

				else if (tmpEdge->semantics == WALL){
					mySpace->_boundedBy.push_back(new AbstractSpaceBoundary());
					//populate the boundaries, no matter if they are navigable or nonnavigable
					mySpace->_boundedBy.back()->geometry.push_back(Point_2(tmpEdge->getStartNode()->geometry.x(),tmpEdge->getStartNode()->geometry.y()));
					mySpace->_boundedBy.back()->geometry.push_back(Point_2(tmpEdge->getEndNode()->geometry.x(),tmpEdge->getEndNode()->geometry.y()));
				}

				
			}



		}



	}//end Area scan


	//for each door space, find its neighbours
	for(vector<AbstractSpace*>::iterator navIt=mybuilding->buildingParts.begin(); navIt!=mybuilding->buildingParts.end(); ++navIt){
		NavigableSpace* navPtr = dynamic_cast <NavigableSpace*>(*navIt);
		if(navPtr != 0){

			//check if it's door type
			Door* doorPtr = dynamic_cast <Door*> (navPtr);
			if (doorPtr != 0) {
				int doorID = doorPtr->id;
				int leftAreaID, rightAreaID;
				if (myTopologicalDataStructure->edges[doorID].leftArea != NULL && myTopologicalDataStructure->edges[doorID].rightArea != NULL){

					leftAreaID = myTopologicalDataStructure->edges[doorID].leftArea->getId();
					rightAreaID = myTopologicalDataStructure->edges[doorID].rightArea->getId();

					for(vector<AbstractSpace*>::iterator scan=mybuilding->buildingParts.begin(); scan!=mybuilding->buildingParts.end(); ++scan){
						if ( (*scan)->id == leftAreaID || (*scan)->id == rightAreaID ) {
							navPtr = dynamic_cast <NavigableSpace*>(*scan);
							if(navPtr != 0){
								doorPtr->addNeighbour(navPtr);
								navPtr->addNeighbour((NavigableSpace*) doorPtr);

							}
						}
					} 
				}
			}
		}
	}
}