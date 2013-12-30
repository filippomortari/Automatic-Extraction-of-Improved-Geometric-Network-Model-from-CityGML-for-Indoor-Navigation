#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <vector>
#include "osmDataFetcher/OsmDataFetcher.h"
#include "topologicalModel/TopologicalDataStructure.h"
#include "config.h"
#include "topologicalModel/node.h"
#include "OgrExporter.h"
#include "control/NetworkGenerator.h"

#include "model/Building.h"
#include "model/Route.h"

#include "cityGMLParser/cityGMLParser.h"



int main()
{
	Building myBuilding;
	Route myRoute;
    
	//OSM
	TopologicalDataStructure myTopologicalDataStructure;
	OgrExporter myOgrExporter;
	OsmDataFetcher myOsmDataFetcher(& myTopologicalDataStructure);
    
	myOsmDataFetcher.parseXML();
	myOsmDataFetcher.extrude(0.00000113);
	myOsmDataFetcher.extrudeDoors(0.00000113);
    
	//myOgrExporter.toShapeFile(& myTopologicalDataStructure);
	//myOgrExporter.vertexToShapeFile(& myTopologicalDataStructure);
	//myOgrExporter.doorToShapeFile(& myTopologicalDataStructure);
	
	myOsmDataFetcher.populateModel(& myBuilding);
	
	myOgrExporter.drawPolygonsModel(myBuilding.buildingParts);
	
	// COMPULSORY NetworkGenerator myNetworkGenerator (& myBuilding, & myRoute);
	// COMPULSORY myNetworkGenerator.generateNetwork();
    
	//CITYGML
	//cityGMLParser myCityGMLParser(& myBuilding);
	//myCityGMLParser.loadFile("cityGMLParser/dataset_example.xml");
	//myCityGMLParser.populateModel();
	
    
	//CITYGMLprint the floor
	/*
     vector<AbstractSpace*> tempRooms;
     for(vector<AbstractSpace*>::iterator it=myBuilding.buildingParts.begin(); it!=myBuilding.buildingParts.end();++it){
     if((*it)->_belongingStorey.front()->storeyNumber==1){
     tempRooms.push_back((*it));
     }
     }
     myOgrExporter.drawPolygonsModel(tempRooms);
     
     
     for(vector<AbstractSpace*>::iterator it=myBuilding.buildingParts.begin(); it!=myBuilding.buildingParts.end();++it){
     cout<<"cityGML id -> "<<(*it)->cityID<<" and room id"<<(*it)->id<<endl;
     if((*it)->spaceType==DOORSPACE){
     double p0,p1,p2,p3;
     p0 = CGAL::to_double((*it)->geometry.vertex(0).x());
     p1 = CGAL::to_double((*it)->geometry.vertex(0).y());
     p2 = CGAL::to_double((*it)->geometry.vertex(1).x());
     p3 = CGAL::to_double((*it)->geometry.vertex(1).y());
     
     cout<<endl<<endl;
     }
     }*/
	
	NetworkGenerator myNetworkGenerator (& myBuilding, & myRoute);
	myNetworkGenerator.generateNetwork();
	myNetworkGenerator.exportRoute(-434);
	
	
	
	
    
    
	system("pause");
	return 0;
}