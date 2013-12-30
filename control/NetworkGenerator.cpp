#include "../control/NetworkGenerator.h"
#include "../print.h"
typedef CGAL::Partition_traits_2<K>                         Traits;


NetworkGenerator::NetworkGenerator(Building* myb, Route * myR){
	myBuild = myb;
	myRoute = myR;
	parameters = EnvironmentParameters::getInstance();
}

NetworkGenerator::~NetworkGenerator(void){

}

void NetworkGenerator::generateNetwork(void){
	//create nodes for door spaces
	for (vector<AbstractSpace*>::iterator it= myBuild->buildingParts.begin(); it!= myBuild->buildingParts.end(); ++it){
		Door* myDoor = dynamic_cast <Door*> (*it);
		if (myDoor!=0) {
			if(myDoor->spaceType == DOORSPACE){
				myRoute->_nodes.push_back(new RouteNode());
				myRoute->_nodes.back()->representedSpace = myDoor;
				myDoor->relatedState = myRoute->_nodes.back();
				myDoor->relatedState->isForNavigation = true;
				myRoute->_nodes.back()->geometry = CGAL::centroid(myDoor->geometry.vertices_begin(),myDoor->geometry.vertices_end());
			}
		}

	}


	//remember: each floor, each room
	AbstractSpace* space;
	NavigableSpace* currentRoom = NULL;
	bool found = false;
	//by now i am only considering the hall space
	for (vector<AbstractSpace*>::iterator it= myBuild->buildingParts.begin(); it!= myBuild->buildingParts.end() && !found; ++it){
		space = (*it);
		//OSM -434 -428 -452
		//CITYGML -15
		if (space->id == -434) 
			found = true;
	}
	//for (vector<AbstractSpace*>::iterator it= myBuild->buildingParts.begin(); it!= myBuild->buildingParts.end(); ++it){
	//	space = (*it);
	currentRoom = dynamic_cast <NavigableSpace*>(space);
	facesWithSemantics.clear();
	crossingSpaces.clear();
	if (currentRoom != 0){
		cout<<"current room "<<currentRoom->id<<endl;
		if (currentRoom->spaceType == GENERALSPACE || currentRoom->spaceType == ANCHORSPACE){

			//do something, single-door room, or door
			Polygon_2 myPoly = currentRoom->geometry;
			if(myPoly.is_convex()){
				myRoute->_nodes.push_back(new RouteNode());
				myRoute->_nodes.back()->representedSpace = currentRoom;
				currentRoom->relatedState = myRoute->_nodes.back();
				currentRoom->relatedState->isForNavigation = true;
				currentRoom->relatedState->geometry = CGAL::centroid(myPoly.vertices_begin(),myPoly.vertices_end());
				myRoute->_edges.push_back(new RouteSegment());
				myRoute->_edges.back()->from = currentRoom->relatedState;
				myRoute->_edges.back()->to = currentRoom->_neighbours.back()->relatedState;
				currentRoom->relatedState->transitions.push_back(myRoute->_edges.back());
				currentRoom->_neighbours.back()->relatedState->transitions.push_back(myRoute->_edges.back());
			}
			//convex decomposition
			else{
				Point_2 midPoint = CGAL::centroid(myPoly.vertices_begin(),myPoly.vertices_end());
				//if the centroid falls inside we might use it as an additional node
				if(myPoly.bounded_side(midPoint) != CGAL::ON_BOUNDED_SIDE){
					list<Traits::Polygon_2> partitionPolys;
					Traits::Polygon_2 partPoly;
					for (VertexIterator vi = myPoly.vertices_begin(); vi!=myPoly.vertices_end();++vi){
						partPoly.push_back(Traits::Point_2((*vi).x(),(*vi).y()));
					}
					CGAL::approx_convex_partition_2(partPoly.vertices_begin(), partPoly.vertices_end(),std::back_inserter(partitionPolys));

					for(list<Traits::Polygon_2>::iterator it=partitionPolys.begin(); it!=partitionPolys.end();++it){
					 //do something
					}
				}
				else{
					myRoute->_nodes.push_back(new RouteNode());
				myRoute->_nodes.back()->representedSpace = currentRoom;
				currentRoom->relatedState = myRoute->_nodes.back();
				currentRoom->relatedState->isForNavigation = true;
				currentRoom->relatedState->geometry = CGAL::centroid(myPoly.vertices_begin(),myPoly.vertices_end());
				myRoute->_edges.push_back(new RouteSegment());
				myRoute->_edges.back()->from = currentRoom->relatedState;
				myRoute->_edges.back()->to = currentRoom->_neighbours.back()->relatedState;
				currentRoom->relatedState->transitions.push_back(myRoute->_edges.back());
				currentRoom->_neighbours.back()->relatedState->transitions.push_back(myRoute->_edges.back());
				}
			}
		}

		else if (currentRoom->spaceType != STAIRSPACE && currentRoom->spaceType != ELEVATORSPACE && currentRoom->spaceType != ESCALATORSPACE && currentRoom->spaceType != RAMPSPACE && currentRoom->spaceType != DOORSPACE){
            print_polygon_on_file(currentRoom->geometry);
			//horizontal transition space, core of the work!
			CDT& triangulatedPolys = currentRoom->triangulatedSpaces;
			vector<Polygon_2> vectPolygons;
			vectPolygons.push_back(currentRoom->geometry);
			FT lOffset = parameters->lOffset;
			if (currentRoom->_neighbours.size()>2){//currentRoom->_neighbours.size()>2
			//generate holes for large environments, by means of inward offset computation
			
			//what if there are holes? use CGAL::create_interior_skeleton_and_offset_polygons_with_holes_2
			PolygonPtrVector offsetPolygons = CGAL::create_exterior_skeleton_and_offset_polygons_2(lOffset,currentRoom->geometry);
			//print_polygons(offsetPolygons);
			
			//for shapefile output
			map <int,list<Point_2> > l;
			list<Point_2> points;
			points.clear();
			int iArea = 0;

			for( PolygonPtrVector::const_iterator pi = offsetPolygons.begin(); pi != offsetPolygons.end() ; ++ pi ){
				iArea++;
				if(abs((**pi).area()) < abs(currentRoom->geometry.area())){
					//it is a proper hole, it can be added to the vector
					//cout<<"inside if: polygon nmr "<<i;
					//print_polygon_on_list((**pi),&points);
					l[iArea] = points;
					points.clear();
					for(VertexIterator verT = (*pi)->vertices_begin(); verT!=(*pi)->vertices_end(); ++verT){
						points.push_back((*verT));
					}
					OgrExporter::printPoly(points,currentRoom->id);
					vectPolygons.push_back((**pi));
				}

			}

			//now we have to perform the sampling of the boundary
			//recall that in case of OSM, the boundary already takes into account the door spaces, this might not be true
			//for CityGML. REMEMBER THAT!
			
			vector <Polygon_2> sampledPolys = computeBoundarySampling(vectPolygons, parameters->samplingRate, currentRoom,(CGAL::to_double(lOffset)*1.8));
			
			computeConstraineDelaunay(sampledPolys, triangulatedPolys);
			}
			else {
				vector <Polygon_2> sampledPolys = computeBoundarySampling(vectPolygons, parameters->samplingRate, currentRoom,(CGAL::to_double(lOffset)*1.8));
				computeConstraineDelaunay(sampledPolys, triangulatedPolys);
			}
			markDomains(triangulatedPolys);
			vector<list<Point_2> > triangulation;
			vector<int> triangulationValues;
			int idTria = 0;

			for (CDT::Finite_faces_iterator fit=triangulatedPolys.finite_faces_begin();fit!=triangulatedPolys.finite_faces_end();++fit){
				if ( fit->info().in_domain() ) {
					idTria++;
					fit->info().id = idTria;
					currentRoom->_subspaces.push_back(new TransitionSpace());
					NavigableSpace* subspace = currentRoom->_subspaces.back();
					fit->info().mySpace = subspace;
					int val = currentRoom->id * 1000;
					fit->info().mySpace->id = val+idTria;
				}
			}

			for (CDT::Finite_faces_iterator fit=triangulatedPolys.finite_faces_begin();fit!=triangulatedPolys.finite_faces_end();++fit){
				if ( fit->info().in_domain() ) {
					//for crossings 3 triangles have to be created
					int ii;
					bool isNotConstrained = false;
					for (ii = 0; ii<3 && !isNotConstrained; ++ii){
						if (fit->is_constrained(ii)==false){
							isNotConstrained = true;
						}
					}
					ii--;
					/*
					cout<<"faccia nr: "<<fit->info().mySpace->id;
					if(fit->is_constrained(0))
					cout<<"vincolo sul lato 0"<<endl;
					if(fit->is_constrained(1))
					cout<<"vincolo sul lato 1"<<endl;
					if(fit->is_constrained(2))
					cout<<"vincolo sul lato 2"<<endl;
					cout<<"il lato non vincolato invece è "<<ii<<endl;
					*/

				}
			}
			/*
			for(vector<AbstractSpaceBoundary*>::iterator spIt=currentRoom->_boundedBy.begin(); spIt!=currentRoom->_boundedBy.end() ; ++spIt){

			cout<<"lato"<<endl;
			if((*spIt)->isNavigable())
			cout<<"anche porta"<<endl<<endl;
			Segment_2 bounSeg = Segment_2((*spIt)->geometry.front(),(*spIt)->geometry.back());
			}*/

			for (CDT::Finite_faces_iterator fit=triangulatedPolys.finite_faces_begin();
				fit!=triangulatedPolys.finite_faces_end();++fit)
			{	
				
				Point_2 p0 = fit->vertex(0)->point();
				Point_2 p1 = fit->vertex(1)->point();
				Point_2 p2 = fit->vertex(2)->point();

				
				list<Point_2> points2;
                
                    points2.push_back(p0);
					points2.push_back(p1);
					points2.push_back(p2);

				if ( fit->info().in_domain() ) {
					//for print purposes
					
					

					bool isHoleNeigh = false;
					bool isHoleNeighbour = false;
					/*
					for (int i = 0; i < 3 && !isHoleNeighbour; ++i){
					if(fit->neighbor(i)->info().nesting_level == 2){
					fit->info().faceType.push_back(HOLENEIGHBOUR);

					}
					}*/
					//detect triangles spanning over a hole
					for (int i = 0; i < 3 && !isHoleNeighbour; ++i){
						Face_circulator fc = triangulatedPolys.incident_faces(fit->vertex(i), fit); 
						if(fc!=0) 
						{ 
							do{ 
								if(fc->info().nesting_level==2){
									fit->info().faceType.push_back(HOLENEIGHBOUR);
									//points.push_back(p0);
									//points.push_back(p1);
									//points.push_back(p2);
									isHoleNeighbour = true;
								}
							}while(++fc!=fit && !isHoleNeighbour); 
						} 
					}

					//crossing
					if (fit->is_constrained(0) == false && fit->is_constrained(1) == false && fit->is_constrained(2) == false){	
						//delete fit->info().mySpace;
						//fit->info().mySpace = NULL;
						// id = 0 would be a marker to recognize this kind of subspace
						fit->info().mySpace->id = 0;
						fit->info().faceType.push_back(CROSSING);
						facesWithSemantics[(int)CROSSING].push_back(fit);
						Polygon_2 poly1, poly2, poly3;
						Point_2 centroid1, centroid2, centroid3;
						list<Point_2> pointsForCentroid;
						pointsForCentroid.clear();
						pointsForCentroid.push_back(p0);
						pointsForCentroid.push_back(p1);
						pointsForCentroid.push_back(p2);
						Point_2 pMid3 = CGAL::centroid(pointsForCentroid.begin(),pointsForCentroid.end(),CGAL::Dimension_tag<0>());

						poly1.push_back(p0);
						poly1.push_back(p1);
						poly1.push_back(pMid3);
						centroid1 = CGAL::midpoint(p0,p1);

						poly2.push_back(p1);
						poly2.push_back(p2);
						poly2.push_back(pMid3);
						centroid2 = CGAL::midpoint(p1,p2);

						poly3.push_back(p2);
						poly3.push_back(p0);
						poly3.push_back(pMid3);
						centroid3 = CGAL::midpoint(p2,p0);

						//create state and transition
						currentRoom->_subspaces.push_back(new TransitionSpace());
						NavigableSpace* subspace = currentRoom->_subspaces.back();
						idTria++;
						int val = currentRoom->id * 1000;
						subspace->id=val+idTria;
						subspace->geometry = poly1;
						subspace->_boundedBy.push_back(new VirtualNavigableBoundary());
						subspace->_boundedBy.back()->geometry.push_back(p0);
						subspace->_boundedBy.back()->geometry.push_back(p1);
						subspace->_boundedBy.push_back(new VirtualNavigableBoundary());
						subspace->_boundedBy.back()->geometry.push_back(p1);
						subspace->_boundedBy.back()->geometry.push_back(pMid3);
						subspace->_boundedBy.push_back(new VirtualNavigableBoundary());
						subspace->_boundedBy.back()->geometry.push_back(pMid3);
						subspace->_boundedBy.back()->geometry.push_back(p0);
						subspace->addNeighbour(fit->neighbor(2)->info().mySpace);
						fit->neighbor(2)->info().mySpace->addNeighbour(subspace);
						myRoute->_nodes.push_back(new RouteNode());
						myRoute->_nodes.back()->geometry = centroid1;
						myRoute->_nodes.back()->representedSpace = subspace;
						subspace->relatedState = myRoute->_nodes.back();

						currentRoom->_subspaces.push_back(new TransitionSpace());
						NavigableSpace* subspace2 = currentRoom->_subspaces.back();
						idTria++;
						val = currentRoom->id * 1000;
						subspace2->id=val+idTria;
						subspace2->geometry = poly2;
						subspace2->_boundedBy.push_back(new VirtualNavigableBoundary());
						subspace2->_boundedBy.back()->geometry.push_back(p1);
						subspace2->_boundedBy.back()->geometry.push_back(p2);
						subspace2->_boundedBy.push_back(new VirtualNavigableBoundary());
						subspace2->_boundedBy.back()->geometry.push_back(p2);
						subspace2->_boundedBy.back()->geometry.push_back(pMid3);
						subspace2->_boundedBy.push_back(new VirtualNavigableBoundary());
						subspace2->_boundedBy.back()->geometry.push_back(pMid3);
						subspace2->_boundedBy.back()->geometry.push_back(p1);
						subspace2->addNeighbour(fit->neighbor(0)->info().mySpace);
						fit->neighbor(0)->info().mySpace->addNeighbour(subspace2);
						myRoute->_nodes.push_back(new RouteNode());
						myRoute->_nodes.back()->geometry = centroid2;
						myRoute->_nodes.back()->representedSpace = subspace2;
						subspace2->relatedState = myRoute->_nodes.back();
						

						currentRoom->_subspaces.push_back(new TransitionSpace());
						NavigableSpace* subspace3 = currentRoom->_subspaces.back();
						idTria++;
						val = currentRoom->id * 1000;
						subspace3->id=val+idTria;
						subspace3->geometry = poly3;
						subspace3->_boundedBy.push_back(new VirtualNavigableBoundary());
						subspace3->_boundedBy.back()->geometry.push_back(p2);
						subspace3->_boundedBy.back()->geometry.push_back(p0);
						subspace3->_boundedBy.push_back(new VirtualNavigableBoundary());
						subspace3->_boundedBy.back()->geometry.push_back(p0);
						subspace3->_boundedBy.back()->geometry.push_back(pMid3);
						subspace3->_boundedBy.push_back(new VirtualNavigableBoundary());
						subspace3->_boundedBy.back()->geometry.push_back(pMid3);
						subspace3->_boundedBy.back()->geometry.push_back(p2);
						subspace3->addNeighbour(fit->neighbor(1)->info().mySpace);
						fit->neighbor(1)->info().mySpace->addNeighbour(subspace3);
						myRoute->_nodes.push_back(new RouteNode());
						myRoute->_nodes.back()->geometry = centroid3;
						myRoute->_nodes.back()->representedSpace = subspace3;
						subspace3->relatedState = myRoute->_nodes.back();
						subspace3->addNeighbour(subspace);
						subspace3->addNeighbour(subspace2);
						subspace->addNeighbour(subspace3);
						subspace2->addNeighbour(subspace3);
						subspace2->addNeighbour(subspace);
						subspace->addNeighbour(subspace2);

						crossingSpaces.push_back(subspace);
						crossingSpaces.push_back(subspace2);
						crossingSpaces.push_back(subspace3);

						fit->info().mySpace->_subspaces.push_back(subspace);
						fit->info().mySpace->_subspaces.push_back(subspace2);
						fit->info().mySpace->_subspaces.push_back(subspace3);

					}//end of CROSSING section

					else{
						Polygon_2 tmPoly;
						tmPoly.push_back(p0);
						tmPoly.push_back(p1);
						tmPoly.push_back(p2);
						//currentRoom->_subspaces.push_back(new TransitionSpace());
						//NavigableSpace* subspaceGeneric = currentRoom->_subspaces.back();
						NavigableSpace* subspaceGeneric = fit->info().mySpace;
						subspaceGeneric->geometry = tmPoly;
						int numConstrainedEdges = 0;
						if (fit->is_constrained(0))
							++numConstrainedEdges;
						if (fit->is_constrained(1))
							++numConstrainedEdges;
						if (fit->is_constrained(2))
							++numConstrainedEdges;

						/*dead end. there can be 3 configurations: 
						-dead end with no doors,
						-dead end with 1 door
						-dead end with 2 doors
						*/
						int counteDoors = 0;
						int ii;
						if(numConstrainedEdges == 2){
							fit->info().faceType.push_back(DEADEND);
							facesWithSemantics[(int) DEADEND].push_back(fit);
							bool isNotConstrained = false;

							for (ii = 0; ii<3 && !isNotConstrained; ++ii){
								if (fit->is_constrained(ii)==false){
									isNotConstrained = true;
									Point_2 pMid1, pMid2, pMid3;
									p0 = fit->vertex(ii)->point();
									p1 = fit->vertex((ii+1)%3)->point();
									p2 = fit->vertex((ii+2)%3)->point();
								}
							}
							ii--;
							subspaceGeneric->_boundedBy.push_back(new VirtualNavigableBoundary());
							subspaceGeneric->_boundedBy.back()->geometry.push_back(p1);
							subspaceGeneric->_boundedBy.back()->geometry.push_back(p2);
							subspaceGeneric->addNeighbour(fit->neighbor(ii)->info().mySpace);
							//cout<<"vicino"<<fit->neighbor(ii)->info().id<<endl;
							//cout<<"nr Tria"<<fit->neighbor(ii)->info().mySpace->id<<endl;
							//fit->neighbor(ii)->info().mySpace->addNeighbour(subspaceGeneric);

							int doorAdjacent = 0;
							Segment_2 doorSeg, navSeg;
							Segment_2 tmpSeg = Segment_2(p0,p1);
							NavigableSpace* roomNeighbour=NULL;
							if(checkIfEdgeisRoomNavigableBoundary(tmpSeg,currentRoom,roomNeighbour)){
								fit->info().faceType.push_back(DOORADJACENT);
								++doorAdjacent;
								subspaceGeneric->_boundedBy.push_back(new NavigableSpaceBoundary());
								subspaceGeneric->_boundedBy.back()->geometry.push_back(p0);
								subspaceGeneric->_boundedBy.back()->geometry.push_back(p1);
								//these to put node in the middle line connecting two segments, one constrained, one not
								doorSeg = Segment_2(p0,p1);
								navSeg = Segment_2(p1,p2);
								//remember to add neighbours
								subspaceGeneric->addNeighbour(roomNeighbour);
								//roomNeighbour->addNeighbour(subspaceGeneric);
								roomNeighbour = NULL;

							}
							else{
								subspaceGeneric->_boundedBy.push_back(new AbstractSpaceBoundary());
								subspaceGeneric->_boundedBy.back()->geometry.push_back(p0);
								subspaceGeneric->_boundedBy.back()->geometry.push_back(p1);
							}

							tmpSeg = Segment_2(p0,p2);
							if(checkIfEdgeisRoomNavigableBoundary(tmpSeg,currentRoom,roomNeighbour)){
								fit->info().faceType.push_back(DOORADJACENT);
								++doorAdjacent;
								//these to put node in the middle line connecting two segments, one constrained, one not
								doorSeg = Segment_2(p0,p2);
								navSeg = Segment_2(p1,p2);
								subspaceGeneric->_boundedBy.push_back(new NavigableSpaceBoundary());
								subspaceGeneric->_boundedBy.back()->geometry.push_back(p0);
								subspaceGeneric->_boundedBy.back()->geometry.push_back(p2);
								//remember to add neighbours
								subspaceGeneric->addNeighbour(roomNeighbour);
								//roomNeighbour->addNeighbour(subspaceGeneric);
								roomNeighbour = NULL;
							}
							else{
								subspaceGeneric->_boundedBy.push_back(new AbstractSpaceBoundary());
								subspaceGeneric->_boundedBy.back()->geometry.push_back(p0);
								subspaceGeneric->_boundedBy.back()->geometry.push_back(p2);
							}
							if(doorAdjacent==0 || doorAdjacent==2){

								myRoute->_nodes.push_back(new RouteNode());
								list<Point_2> pointsForCentroid;
								pointsForCentroid.clear();
								pointsForCentroid.push_back(p0);
								pointsForCentroid.push_back(p1);
								pointsForCentroid.push_back(p2);
								Point_2 pMid3 = CGAL::centroid(pointsForCentroid.begin(),pointsForCentroid.end(),CGAL::Dimension_tag<0>());
								myRoute->_nodes.back()->geometry = pMid3;
								myRoute->_nodes.back()->representedSpace = subspaceGeneric;
								subspaceGeneric->relatedState = myRoute->_nodes.back();
							}
							else{
								myRoute->_nodes.push_back(new RouteNode());
								Point_2 pMid1, pMid2, pMid3;
								pMid1 = CGAL::midpoint(doorSeg.source(),doorSeg.target());
								pMid2 = CGAL::midpoint(navSeg.source(),navSeg.target());
								pMid3 = CGAL::midpoint(pMid1,pMid2);
								myRoute->_nodes.back()->geometry = pMid3;
								myRoute->_nodes.back()->representedSpace = subspaceGeneric;
								subspaceGeneric->relatedState = myRoute->_nodes.back();
							}

						}//dead end 


						//it might be a door triangle
						else if(numConstrainedEdges ==1){
							bool isConstrained = false;
							for (ii = 0; ii<3 && !isConstrained; ++ii){
								if (fit->is_constrained(ii)==true){
									isConstrained = true;
									p0 = fit->vertex(ii)->point();
									p1 = fit->vertex((ii+1)%3)->point();
									p2 = fit->vertex((ii+2)%3)->point();
								}
							}
							ii--;

							//first set the door information
							Segment_2 tmpSeg = Segment_2(p1,p2);
							NavigableSpace* roomNeighbour=NULL;
							if(checkIfEdgeisRoomNavigableBoundary(tmpSeg,currentRoom,roomNeighbour)){
								fit->info().faceType.push_back(DOORADJACENT);
								subspaceGeneric->_boundedBy.push_back(new NavigableSpaceBoundary());
								subspaceGeneric->_boundedBy.back()->geometry.push_back(p1);
								subspaceGeneric->_boundedBy.back()->geometry.push_back(p2);
								//remember to add neighbours
								subspaceGeneric->addNeighbour(roomNeighbour);
								//roomNeighbour->addNeighbour(subspaceGeneric);
								roomNeighbour = NULL;

								//points2.push_back(p0);
								//points2.push_back(p1);
								//points2.push_back(p2);
							}
							else{
								subspaceGeneric->_boundedBy.push_back(new AbstractSpaceBoundary());
								subspaceGeneric->_boundedBy.back()->geometry.push_back(p1);
								subspaceGeneric->_boundedBy.back()->geometry.push_back(p2);
							}
							subspaceGeneric->_boundedBy.push_back(new VirtualNavigableBoundary());
							subspaceGeneric->_boundedBy.back()->geometry.push_back(p0);
							subspaceGeneric->_boundedBy.back()->geometry.push_back(p1);
							subspaceGeneric->_boundedBy.push_back(new VirtualNavigableBoundary());
							subspaceGeneric->_boundedBy.back()->geometry.push_back(p2);
							subspaceGeneric->_boundedBy.back()->geometry.push_back(p0);
							//fit->neighbor((ii+1)%3)->info().mySpace->addNeighbour(subspaceGeneric);
							//fit->neighbor((ii+2)%3)->info().mySpace->addNeighbour(subspaceGeneric);
							subspaceGeneric->addNeighbour(fit->neighbor((ii+1)%3)->info().mySpace);
							subspaceGeneric->addNeighbour(fit->neighbor((ii+2)%3)->info().mySpace);
							myRoute->_nodes.push_back(new RouteNode());
							Point_2 pMid1, pMid2, pMid3;
							pMid1 = CGAL::midpoint(p0,p1);
							pMid2 = CGAL::midpoint(p0,p2);
							pMid3 = CGAL::midpoint(pMid1,pMid2);
							myRoute->_nodes.back()->geometry = pMid3;
							myRoute->_nodes.back()->representedSpace = subspaceGeneric;
							subspaceGeneric->relatedState = myRoute->_nodes.back();

						}


						numConstrainedEdges = 0;
					}
					


				}//fit in domain




				triangulation.push_back(points2);
				triangulationValues.push_back(fit->info().id);
				points2.clear();

			}
			//print
			OgrExporter::drawTriangulation(triangulation,currentRoom->id,triangulationValues);
			//placeNodesInTriangles(triangulatedPolys);

			/*stampa solo porte
			list<Point_2> lista;
			for(vector<AbstractSpaceBoundary*>::iterator it=currentRoom->_boundedBy.begin();it!=currentRoom->_boundedBy.end();++it){
			NavigableSpaceBoundary* nav = dynamic_cast<NavigableSpaceBoundary*>(*it);
			if(nav!=0){
			lista.push_back((*it)->geometry.front());
			lista.push_back((*it)->geometry.back());
			}
			}
			//OgrExporter::printVertices(lista,currentRoom->id);*/

			for (CDT::Finite_faces_iterator fit=triangulatedPolys.finite_faces_begin();fit!=triangulatedPolys.finite_faces_end();++fit){
				if ( fit->info().in_domain() ) {
					if (fit->is_constrained(0) == false && fit->is_constrained(1) == false && fit->is_constrained(2) == false){
						//should delete from dynamic memory the subspaces created for the crossings
					}
				}
			}
			/*
			for(vector<NavigableSpace*>::iterator it=currentRoom->_subspaces.begin(); it!=currentRoom->_subspaces.end();++it){
			if((*it)->id == 0){
			delete * it;  
			it = currentRoom->_subspaces.erase(it);
			}
			else{
			++it;
			}
			}*/

			//OgrExporter::drawTriangulation(triangulation,currentRoom->id,triangulationValues);
			linkNodes(myRoute, triangulatedPolys);
			if(currentRoom->_neighbours.size()>2)
				studyHolesForAdditionalNodes(vectPolygons, triangulatedPolys, currentRoom);
			//print nodes, and triangulation
			triangulation.clear();
			Polygon_2 myNodes;
			for(vector<RouteNode*>::iterator it= myRoute->_nodes.begin(); it!=myRoute->_nodes.end(); ++it){
				list<Point_2>vertices;
				Polygon_2 myPoly = (*it)->representedSpace->geometry;
				if((*it)->isForNavigation)
					myNodes.push_back((*it)->geometry);
				for(VertexIterator vi=myPoly.vertices_begin(); vi!=myPoly.vertices_end();++vi){
					vertices.push_back((*vi));
				}
				triangulation.push_back(vertices);
				vertices.clear();
			}
			OgrExporter::printVertices(myNodes,10000);
			OgrExporter::drawTriangulation(triangulation,currentRoom->id,triangulationValues);

			//print transition
			vector <Segment_2> transForPrint;
			for(vector<RouteSegment*>::iterator it = myRoute->_edges.begin(); it!= myRoute->_edges.end();++it){
				transForPrint.push_back((*it)->geometry);
			}

			OgrExporter::drawTransitions(transForPrint,currentRoom->id);
		}

		else if (currentRoom->spaceType == STAIRSPACE || currentRoom->spaceType == ELEVATORSPACE || currentRoom->spaceType == ESCALATORSPACE || currentRoom->spaceType == RAMPSPACE){
			//do something for vertical passages
		}
	}//end check on currentRoom pointer
//}//cancel to remove the loop
}

vector<Polygon_2> NetworkGenerator::computeBoundarySampling(vector<Polygon_2> polys, double sampleRate, NavigableSpace* tmpSpace, double lOffset){
	vector<Polygon_2> result;
	Polygon_2 poly2;
	Point_2 pStart, pEnd, pFirst, pLast, tmpCurrent,tmpLast;
	double x,y, tmpSrcX, tmpTrgtX, tmpSrcY, tmpTrgtY, div;
	Polygon_2 listPrint;
	bool isFoundNavigable = false;
	for(vector<Polygon_2>::iterator polyIt=polys.begin(); polyIt!=polys.end(); ++polyIt){
		poly2 = (*polyIt);
		Polygon_2 tmPoly, tmPolyNoRep;
		//print_polygon(poly2);
		for(EdgeIterator ei = poly2.edges_begin(); ei != poly2.edges_end(); ++ei){
			if (ei == poly2.edges_begin())
				pFirst = (*ei).source();
			pStart = (*ei).source();
			pEnd = (*ei).end();

			pLast = pEnd;
			Segment_2 tmpSeg = Segment_2(pStart,pEnd);
			isFoundNavigable = false;
			NavigableSpace* navi= NULL;
			//only the first polygon appeals to room boundary, and only boundaries contain doors
			if(polyIt==polys.begin()){
				isFoundNavigable = checkIfEdgeisRoomNavigableBoundary(tmpSeg,tmpSpace,navi);
			}
			int numTimes; 
			//cout<<" distanza "<<sqrt(tmpSeg.squared_length())<<endl;

			if((sqrt(CGAL::to_double(tmpSeg.squared_length())) > sampleRate && !isFoundNavigable && (sqrt(CGAL::to_double(tmpSeg.squared_length()))) > lOffset))
				numTimes = sqrt(CGAL::to_double(tmpSeg.squared_length())) / sampleRate;
			else 
				numTimes = 1;
			if (numTimes == 0)
				numTimes = 1;
			//cout<<"boh"<<" distanza "<<sqrt(tmpSeg.squared_length())/sampleRate<<endl;
			//cout<<"divido :"<<numTimes;
			bool avoidPoint = false;
			int i;
			for (i= 0; i<=numTimes; i++){
				tmpSrcX = CGAL::to_double(tmpSeg.source().x());
				tmpSrcY = CGAL::to_double(tmpSeg.source().y());
				tmpTrgtX = CGAL::to_double(tmpSeg.target().x());
				tmpTrgtY = CGAL::to_double(tmpSeg.target().y());
				div = (float)i/(float)numTimes;
				x = tmpSrcX + (div)*(tmpTrgtX-tmpSrcX);
				y = tmpSrcY + (div)*(tmpTrgtY-tmpSrcY);
				//we don't want to have the first point duplicated
				if (ei == poly2.edges_begin()){
					//tmpLast = Point_2(0,0);
				}
				else
					tmpLast = pStart;
				//first one should be different from last vertex
				if (pFirst == pLast && i==numTimes)
					avoidPoint = true;
				//avoid duplicates 
				tmpCurrent = Point_2(x,y);
				if(!avoidPoint && tmpLast!=tmpCurrent)
					tmPoly.push_back(Point_2(x,y));




				//only for print
				listPrint.push_back(Point_2(x,y));
			}

		}
		print_polygon(tmPoly);
		Point_2 pInit, pEnding;
		for(VertexIterator vi=tmPoly.vertices_begin(); vi!=tmPoly.vertices_end();++vi){

			if (vi==tmPoly.vertices_begin()){
				pEnding = (*vi);
				pInit = (*vi);
				tmPolyNoRep.push_back(pEnding);
				//print_point(pEnding);
				//cout<<endl;
			}
			else 
				pEnding = pInit;
			pInit = (*vi);
			/*cout<<"pInit: ";
			print_point(pInit);
			cout<<endl;
			cout<<"pEnding: ";
			print_point(pEnding);
			cout<<endl;*/
			double x1, x2, y1,y2;
			x1 = CGAL::to_double(pInit.x());
			x2 = CGAL::to_double(pEnding.x());
			y1 = CGAL::to_double(pInit.x());
			y2 = CGAL::to_double(pEnding.x());
			//dunno why the operator == between two Point_2 gives wrong results
			double precision = parameters->precision;
			if(sqrt(CGAL::to_double (CGAL::squared_distance(pInit,pEnding)))>precision){
				//cout<<"ho inserito: ";
				//print_point(pInit);cout<<endl;
				tmPolyNoRep.push_back(pInit);


			}
			else {
				//++vi;
			}
		}
		//print_polygon(tmPolyNoRep);
		//some duplicate vertices still exist, why? lets delete them
		/*list<VertexIterator> wrongVertices;
		for(VertexIterator vi= tmPolyNoRep.vertices_begin(); vi!=tmPolyNoRep.vertices_end(); ++vi){
		Point_2 p1, p2;
		p1 = (*vi);
		for(VertexIterator viPlus= tmPolyNoRep.vertices_begin(); viPlus!=tmPolyNoRep.vertices_end(); ++viPlus){
		p2 = (*viPlus);
		if(p1 == p2 && vi != viPlus){
		cout<<"duplicato"<<endl;

		wrongVertices.push_back(viPlus);
		//tmPoly.erase(viPlus);
		}
		}

		}
		cout<<"i vertici duplicati: "<<wrongVertices.size()<<endl;
		wrongVertices.clear();*/
		/*
		for(list<VertexIterator>::iterator i= wrongVertices.begin(); i!= wrongVertices.end(); ++i){
		tmPoly.erase((*i));
		}*/
		//cout<<"dopo il sampling"<<endl;
		print_polygon(tmPolyNoRep);
		result.push_back(tmPolyNoRep);
	}
    cout<<"provo a stampare la x "<<CGAL::to_double(listPrint.vertex(0).x())<<endl;
	OgrExporter::printVertices(listPrint,-123456);
	return result;
}

bool NetworkGenerator::checkIfEdgeisRoomNavigableBoundary(Segment_2 roomSeg, NavigableSpace* tmpSpace, NavigableSpace*& theNeighbour){
	bool isFoundNavigable = false;
	for(vector<AbstractSpaceBoundary*>::iterator spIt=tmpSpace->_boundedBy.begin(); spIt!=tmpSpace->_boundedBy.end() && !isFoundNavigable; ++spIt){
		if((*spIt)->isNavigable()){
			//pay attention, in CityGML it may be not a 2 point segment!!!
			Segment_2 bounSeg = Segment_2((*spIt)->geometry.front(),(*spIt)->geometry.back());
			/*not only intersect, they have to share a segment, now I am doing like this, in future it might be better using GEOS suite 
			/*for topological relationships
			*/

			/*
			if(CGAL::do_intersect(roomSeg,bounSeg)){
			CGAL::Object obj = CGAL::intersection(roomSeg,bounSeg);
			if (const Segment_2 *segment = CGAL::object_cast<Segment_2>(&obj)) {
			isFoundNavigable = true;
			}

			}*/
			if(approxIntersection(roomSeg,bounSeg)){
				isFoundNavigable = true;
				bool foundRoom = false;
				for(vector<NavigableSpace*>::iterator it=tmpSpace->_neighbours.begin(); it!=tmpSpace->_neighbours.end() && !foundRoom; ++it){
					for(vector<AbstractSpaceBoundary*>::iterator drIt=(*it)->_boundedBy.begin(); drIt!=(*it)->_boundedBy.end() && !foundRoom;++drIt){
						if((*drIt)->isNavigable()){
							Segment_2 doorSeg = Segment_2((*drIt)->geometry.front(),(*drIt)->geometry.back());
							if(approxIntersection(bounSeg,doorSeg)){
								foundRoom = true;
								theNeighbour = (*it);
							}
						}
					}

				}
			}
		}
	}
	return isFoundNavigable;
}

void NetworkGenerator::computeConstraineDelaunay(vector<Polygon_2> polys, CDT& cdt){
	for (vector<Polygon_2>::iterator it=polys.begin(); it!=polys.end(); ++it){
		insertPolygonIntoCDT(cdt,(*it));
	}
}

void NetworkGenerator::insertPolygonIntoCDT(CDT& cdt,const Polygon_2& polygon){

	if ( polygon.is_empty() ) return;
	CDT::Vertex_handle v_prev=cdt.insert(*CGAL::cpp11::prev(polygon.vertices_end()));
	for (Polygon_2::Vertex_iterator vit=polygon.vertices_begin();
		vit!=polygon.vertices_end();++vit)
	{
		CDT::Vertex_handle vh=cdt.insert(*vit);
		cdt.insert_constraint(vh,v_prev);
		v_prev=vh;
	}
}

void NetworkGenerator::markDomains(CDT& cdt)
{
	for(CDT::All_faces_iterator it = cdt.all_faces_begin(); it != cdt.all_faces_end(); ++it){
		it->info().nesting_level = -1;
	}

	int index = 0;
	std::list<CDT::Edge> border;
	markDomains(cdt, cdt.infinite_face(), index++, border);
	while(! border.empty()){
		CDT::Edge e = border.front();
		border.pop_front();
		CDT::Face_handle n = e.first->neighbor(e.second);
		if(n->info().nesting_level == -1){
			markDomains(cdt, n, e.first->info().nesting_level+1, border);
		}
	}
}

void NetworkGenerator::markDomains(CDT& ct, CDT::Face_handle start, int index, std::list<CDT::Edge>& border )
{
	if(start->info().nesting_level != -1){
		return;
	}
	std::list<CDT::Face_handle> queue;
	queue.push_back(start);

	while(! queue.empty()){
		CDT::Face_handle fh = queue.front();
		queue.pop_front();
		if(fh->info().nesting_level == -1){
			fh->info().nesting_level = index;
			for(int i = 0; i < 3; i++){
				CDT::Edge e(fh,i);
				CDT::Face_handle n = fh->neighbor(i);
				if(n->info().nesting_level == -1){
					if(ct.is_constrained(e)) 
						border.push_back(e);
					else 
						queue.push_back(n);
				}
			}
		}
	}
}

bool NetworkGenerator::approxIntersection(Segment_2 s1, Segment_2 s2){
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

//NOT USED ANYMORE
void NetworkGenerator::placeNodesInTriangles(CDT& myTriangulation){
	Polygon_2 listPrintout;
	for (CDT::Finite_faces_iterator faceIT=myTriangulation.finite_faces_begin();faceIT!=myTriangulation.finite_faces_end();++faceIT){
		int numConstrainedEdges = 0;
		for (int i = 0; i<3; ++i){
			if (faceIT->is_constrained(i) && faceIT->info().in_domain())
				++numConstrainedEdges;
		}
		//only one edge is constrained
		if (numConstrainedEdges == 1){
			for (int i = 0; i<3; ++i){
				if (faceIT->is_constrained(i) && faceIT->info().in_domain()){
					Point_2 p0, p1, p2, pMid1, pMid2, pMid3;
					p0 = faceIT->vertex(i)->point();
					p1 = faceIT->vertex((i+1)%3)->point();
					p2 = faceIT->vertex((i+2)%3)->point();
					pMid1 = CGAL::midpoint(p0,p1);
					pMid2 = CGAL::midpoint(p0,p2);
					pMid3 = CGAL::midpoint(pMid1,pMid2);
					listPrintout.push_back(pMid3);
				}

			}
		}
		else if((numConstrainedEdges == 0 || numConstrainedEdges == 2 )&&faceIT->info().in_domain()){
			list<Point_2> pointsForCentroid;
			pointsForCentroid.clear();
			pointsForCentroid.push_back(faceIT->vertex(0)->point());
			pointsForCentroid.push_back(faceIT->vertex(1)->point());
			pointsForCentroid.push_back(faceIT->vertex(2)->point());
			Point_2 pMid3 = CGAL::centroid(pointsForCentroid.begin(),pointsForCentroid.end(),CGAL::Dimension_tag<0>());
			listPrintout.push_back(pMid3);
		}
	}

	OgrExporter::printVertices(listPrintout,1000);
}

void NetworkGenerator::linkNodes(Route* r, CDT& myTriangulation){
	int count = 0;
	list<CDT::Face_handle> deadNodes = facesWithSemantics[(int)DEADEND];
	for(list<CDT::Face_handle>::iterator it= deadNodes.begin(); it!=deadNodes.end();++it){
		count++;
		//cout<<"deadNodes size= "<<deadNodes.size()<<" and count is: "<<count<<endl;
		bool notDeadEnd = false;
		for (list<FaceType>::iterator spIt=(*it)->info().faceType.begin(); spIt!=(*it)->info().faceType.end(); ++spIt){
			if ((*spIt) == CROSSING || (*spIt) == DOORADJACENT)
				notDeadEnd = true;
		}
		if(!notDeadEnd){
			(*it)->info().mySpace->relatedState->isForNavigation = false;
			bool isImportantNode = false;
			CDT::Face_handle neighbourFace,currentFace;
			currentFace = (*it);
			for(int i=0; i<3; ++i){
				if((*it)->is_constrained(i)==false)
					neighbourFace=(*it)->neighbor(i);
			}

			while (isImportantNode==false){
				/*print_point(neighbourFace->vertex(0)->point());
				cout<<endl;
				print_point(neighbourFace->vertex(1)->point());
				cout<<endl;
				print_point(neighbourFace->vertex(2)->point());
				cout<<endl<<endl<<endl;*/
				for (list<FaceType>::iterator spIt=neighbourFace->info().faceType.begin(); spIt!=neighbourFace->info().faceType.end(); ++spIt){

					if ((*spIt) == CROSSING || (*spIt) == DOORADJACENT)
						isImportantNode = true;
				}
				if (isImportantNode ==false) {
					bool stop = false;
					for (int k=0; k<3 && !stop; ++k){
						if (neighbourFace->is_constrained(k) == false && neighbourFace->neighbor(k)!= currentFace){
							currentFace = neighbourFace;
							neighbourFace = neighbourFace->neighbor(k);
							currentFace->info().mySpace->relatedState->isForNavigation = false;
							stop = true;
						}
					}
				}
			}
		}
	}
	//if a crossing has appended some dead routes that lead to the corner of a room, remove the representative point
	for(list<NavigableSpace*>::iterator it=crossingSpaces.begin(); it!=crossingSpaces.end();++it){
		Point_2 pMid = CGAL::centroid((*it)->geometry.vertices_begin(),(*it)->geometry.vertices_end());
		CDT::Face_handle fa = myTriangulation.locate(pMid);
		//print_point(pMid);
		Segment_2 s1,s2,s3;
		s1 = Segment_2(fa->vertex(0)->point(),fa->vertex(1)->point());
		s2 = Segment_2(fa->vertex(1)->point(),fa->vertex(2)->point());
		s3 = Segment_2(fa->vertex(2)->point(),fa->vertex(0)->point());
		Polygon_2 poly;
		poly.push_back(fa->vertex(0)->point());
		poly.push_back(fa->vertex(1)->point());
		poly.push_back(fa->vertex(2)->point());
		bool isFound=false;
		for (vector<NavigableSpace*>::iterator sbIt = (*it)->_neighbours.begin(); sbIt!= (*it)->_neighbours.end() && !isFound; ++sbIt){
			if((*sbIt)->id!=0 && (*sbIt)->relatedState->isForNavigation==false){
				list<NavigableSpace*>::iterator presentSpace = std::find(crossingSpaces.begin(),crossingSpaces.end(),(*sbIt));
				if(presentSpace == crossingSpaces.end()){
					for(vector<AbstractSpaceBoundary*>::iterator bounIt= (*it)->_boundedBy.begin(); bounIt != (*it)->_boundedBy.end() && !isFound; ++bounIt){
						Segment_2 sBoun = Segment_2((*bounIt)->geometry.back(),(*bounIt)->geometry.front());
						if((approxIntersection(s1,sBoun) || approxIntersection(s2,sBoun)||approxIntersection(s3,sBoun))){
							(*it)->relatedState->isForNavigation = false;
							isFound = true;

						}
					}
				}
			}
		}
	}
	//polish the remaining subspace of crossing attached to 2 deadends
	for(list<NavigableSpace*>::iterator croIt = crossingSpaces.begin(); croIt!=crossingSpaces.end(); ++croIt){
		Point_2 pMid = CGAL::centroid((*croIt)->geometry.vertices_begin(),(*croIt)->geometry.vertices_end());
		CDT::Face_handle fa = myTriangulation.locate(pMid);
		NavigableSpace* cross = fa->info().mySpace;
		int numfaces=0;
		NavigableSpace* potentialSpace= NULL;
		for(vector<NavigableSpace*>::iterator subIt = cross->_subspaces.begin(); subIt!= cross->_subspaces.end(); ++subIt){
			if((*subIt)->relatedState->isForNavigation==false){
				++numfaces;
			}
			else
				potentialSpace = (*subIt);
		}
		if (numfaces == 2)
			potentialSpace->relatedState->isForNavigation = false;

	}

	//create transitions
	/*map<int,bool> isVisited;
	for(vector<RouteNode*>::iterator it=myRoute->_nodes.begin();it!=myRoute->_nodes.end();++it){
		isVisited[(*it)->representedSpace->id] = false;
		AbstractSpace* abs = (*it)->representedSpace;
		NavigableSpace* myNav = dynamic_cast <NavigableSpace*> (abs);
		cout<<"number of neighbours for space "<<(*it)->representedSpace->id<<"= "<<myNav->_neighbours.size()<<endl;
		cout<<"space "<<(*it)->representedSpace->id<<"is navigable: "<<(*it)->isForNavigation<<endl;
	}*/

	for(vector<RouteNode*>::iterator it=myRoute->_nodes.begin();it!=myRoute->_nodes.end();++it){
		if (true){//isVisited[(*it)->representedSpace->id] == false
			//isVisited[(*it)->representedSpace->id] = true;
			AbstractSpace* abs = (*it)->representedSpace;
			NavigableSpace* myNav = dynamic_cast <NavigableSpace*> (abs);
			if (myNav!=0 && myNav->spaceType!=DOORSPACE && myNav->relatedState->isForNavigation){
				for(vector<NavigableSpace*>::iterator neigIt=myNav->_neighbours.begin(); neigIt!=myNav->_neighbours.end();++neigIt){
					if((*neigIt) !=0 ){//isVisited[(*neigIt)->id] == false && 
						
						if((*neigIt)->id!=0 && (*neigIt)->relatedState->isForNavigation ){
							//isVisited[(*neigIt)->id] = true;
							myRoute->_edges.push_back(new RouteSegment());
							RouteSegment* routeSeg = myRoute->_edges.back();
							routeSeg->from = (*it);
							routeSeg->to = (*neigIt)->relatedState;
							(*it)->transitions.push_back(routeSeg);
							//(*neigIt)->relatedState->transitions.push_back(routeSeg);
							routeSeg->geometry = Segment_2((*it)->geometry,(*neigIt)->relatedState->geometry);
						}
					}
				}
			}
		}
	}

}

void NetworkGenerator::studyHolesForAdditionalNodes(vector<Polygon_2> vectPolygons, CDT& cdt, NavigableSpace* currentRoom){
	vector<list<Traits::Polygon_2> > partitionPolys;
	vector<Traits::Polygon_2> offsetPolys;
	for(vector<Polygon_2>::iterator polIt=vectPolygons.begin(); polIt!=vectPolygons.end(); ++polIt){
		Traits::Polygon_2 partPoly;
		for (VertexIterator vi = (*polIt).vertices_begin(); vi!=(*polIt).vertices_end();++vi){
			partPoly.push_back(Traits::Point_2(CGAL::to_double((*vi).x()),CGAL::to_double((*vi).y())));
		}
		//first poly is the boundary of the room
		if(polIt!=vectPolygons.begin()){
			//cout<<endl<<endl;
			//cout << "The polygon is " <<
				//(partPoly.is_simple() ? "" : "not ") << "simple." << endl;
			
			offsetPolys.push_back(partPoly);
		}

	}


	int count = 0;
	for(vector<Traits::Polygon_2>::iterator it=offsetPolys.begin(); it!=offsetPolys.end();++it){
		++count;
		list<Traits::Polygon_2> result; 
		Point_2 midPoint = CGAL::centroid((*it).vertices_begin(), (*it).vertices_end());
		//if the centroid falls inside we might use it as an additional node
		if((*it).bounded_side(midPoint) != CGAL::ON_BOUNDED_SIDE){
			if((*it).is_clockwise_oriented())
				(*it).reverse_orientation();
			CGAL::optimal_convex_partition_2((*it).vertices_begin(), (*it).vertices_end(),std::back_inserter(result));
			partitionPolys.push_back(result);
		}
		else{
			Polygon_2 lista;
			lista.push_back(midPoint);
			OgrExporter::printVertices(lista,-130188);
			myRoute->_nodes.push_back(new RouteNode());
			RouteNode* centerNode = myRoute->_nodes.back();
			centerNode->geometry = midPoint;
			centerNode->isForNavigation = true;
			currentRoom->_subspaces.push_back(new TransitionSpace());
			centerNode->representedSpace = currentRoom->_subspaces.back();
			centerNode->representedSpace->geometry = vectPolygons[count];
			centerNode->representedSpace->relatedState = centerNode;
			for(CDT::Finite_faces_iterator fit=cdt.finite_faces_begin();
				fit!=cdt.finite_faces_end();++fit)
			{	
				list<Point_2> points2;
				Point_2 p0 = fit->vertex(0)->point();
				Point_2 p1 = fit->vertex(1)->point();
				Point_2 p2 = fit->vertex(2)->point();

				if ( fit->info().in_domain() ) {
					bool holeNeighbour = false;
					bool doorAdjacent = false;
					bool crossing = false;
					for (list<FaceType>::iterator iter=fit->info().faceType.begin(); iter!=fit->info().faceType.end();++iter){
						if((*iter)==HOLENEIGHBOUR)
							holeNeighbour = true;
						if((*iter)==DOORADJACENT)
							doorAdjacent= true;
						if((*iter)==CROSSING)
							crossing = true;
					}
					if(holeNeighbour==true && doorAdjacent == true){
						NavigableSpace* currSpace = fit->info().mySpace;
						currSpace->addNeighbour(currentRoom->_subspaces.back());
						myRoute->_edges.push_back(new RouteSegment());
						RouteSegment* curRouteSeg = myRoute->_edges.back();
						curRouteSeg->from = currSpace->relatedState;
						curRouteSeg->to = centerNode;
						currSpace->relatedState->transitions.push_back(curRouteSeg);
						centerNode->transitions.push_back(curRouteSeg);
						curRouteSeg->geometry = Segment_2(curRouteSeg->from->geometry,curRouteSeg->to->geometry);
					}

					else if(holeNeighbour==true && crossing == true){
						Segment_2 segMin, segCurr;
						NavigableSpace* currSpace = fit->info().mySpace;
						NavigableSpace* minSpace;
						//since crossing has 3 nodes, choose the one that minimizes the distance to centroid
						for (vector<NavigableSpace*>::iterator navIT = currSpace->_subspaces.begin(); navIT != currSpace->_subspaces.end(); ++navIT){
							if (navIT==currSpace->_subspaces.begin()){
								segMin = Segment_2(midPoint,(*navIT)->relatedState->geometry);
								minSpace = (*navIT);
							}
								
							segCurr = Segment_2(midPoint,(*navIT)->relatedState->geometry);
							if (segCurr.squared_length()<segMin.squared_length()){
								segMin = segCurr;
								minSpace = (*navIT);
							}
								
						}
						if (minSpace->relatedState->isForNavigation){
							minSpace->addNeighbour(currentRoom->_subspaces.back());
							myRoute->_edges.push_back(new RouteSegment());
							RouteSegment* curRouteSeg = myRoute->_edges.back();
							curRouteSeg->from = minSpace->relatedState;
							curRouteSeg->to = centerNode;
							minSpace->relatedState->transitions.push_back(curRouteSeg);
							centerNode->transitions.push_back(curRouteSeg);
							curRouteSeg->geometry = segMin;
						}
					}
				}
			}
		}
	}

	for(vector<list<Traits::Polygon_2> >::iterator it= partitionPolys.begin(); it!=partitionPolys.end();++it){
		for(list<Traits::Polygon_2>::iterator currPoly = (*it).begin(); currPoly!=(*it).end(); ++currPoly){
			Traits::Polygon_2 currentPoly = (*currPoly);
			OgrExporter::printPoly(currentPoly,0);
		}
	}
}

void NetworkGenerator::exportRoute(int idRoom){
	//NODES
	string resultName;
	do{
		int random = rand() % 100;
		string fileName ("Nodes");
		string extension(".txt");
		std::stringstream sstm;
		sstm << fileName <<"_"<< random << extension;
		resultName = sstm.str();
	}
	while(OgrExporter::fileExists(resultName));

	string resultName2;
	do{
		int random = rand() % 100;
		string fileName ("Transitions");
		string extension(".txt");
		std::stringstream sstm;
		sstm << fileName <<"_"<< random << extension;
		resultName2 = sstm.str();
	}
	while(OgrExporter::fileExists(resultName2));

	ofstream fileNodes;
	fileNodes.precision(16);
	fileNodes.open (resultName.c_str());
	fileNodes << "ID, X, Y, IsDoor"<<endl;

	ofstream fileTrans;
	fileTrans.precision(16);
	fileTrans.open (resultName2.c_str());
	fileTrans << "node1, node2, Distance"<<endl;
	//stupid value that i will use to output the whole building / storey
	if(idRoom != 12345678){
		AbstractSpace* space = NULL;
		bool found = false;
		for (vector<AbstractSpace*>::iterator it= myBuild->buildingParts.begin(); it!= myBuild->buildingParts.end() && !found; ++it){
			space = (*it);

			if (space->id == idRoom) 
				found = true;
		}
		int count = 0;
		NavigableSpace* currentRoom = dynamic_cast <NavigableSpace*>(space);
		for(vector<NavigableSpace*>::iterator subSp = currentRoom->_subspaces.begin(); subSp != currentRoom->_subspaces.end(); ++subSp){
			
			if((*subSp)->id != 0){
				bool isDoor;
				count++;
				if ((*subSp)->spaceType == DOORSPACE) 
					isDoor = true;
				else 
					isDoor = false;
				if ((*subSp)->relatedState->isForNavigation){
					fileNodes <<(*subSp)->relatedState->id<<", "<<CGAL::to_double((*subSp)->relatedState->geometry.x())
						<<", "<<CGAL::to_double((*subSp)->relatedState->geometry.y())<<", "<<isDoor<<endl;
					for(vector<Transition*>::iterator transitions = (*subSp)->relatedState->transitions.begin(); transitions != (*subSp)->relatedState->transitions.end(); ++transitions){
						State* tempNode;
						if((*transitions)->from == (*subSp)->relatedState){
							tempNode = (*transitions)->to;
						}
						else
							tempNode = (*transitions)->from;
						fileTrans<<(*subSp)->relatedState->id<<", "<<tempNode->id<<", "<<sqrt(CGAL::to_double(CGAL::squared_distance((*subSp)->relatedState->geometry,tempNode->geometry)))<<endl;
					}
				}
					
				//cout <<(*subSp)->relatedState->id<<", "<<CGAL::to_double((*subSp)->relatedState->geometry.x())
					//<<", "<<CGAL::to_double((*subSp)->relatedState->geometry.y())<<", "<<isDoor<<endl;
			}
		}
			//DOORS
		for(vector<NavigableSpace*>::iterator subSp = currentRoom->_neighbours.begin(); subSp != currentRoom->_neighbours.end(); ++subSp){
			bool isDoor;
			count++;
			if ((*subSp)->spaceType == DOORSPACE) 
				isDoor = true;
			else 
				isDoor = false;
			if ((*subSp)->relatedState->isForNavigation){
					fileNodes <<(*subSp)->relatedState->id<<", "<<CGAL::to_double((*subSp)->relatedState->geometry.x())
						<<", "<<CGAL::to_double((*subSp)->relatedState->geometry.y())<<", "<<isDoor<<endl;
					for(vector<Transition*>::iterator transitions = (*subSp)->relatedState->transitions.begin(); transitions != (*subSp)->relatedState->transitions.end(); ++transitions){
						State* tempNode;
						if((*transitions)->from == (*subSp)->relatedState){
							tempNode = (*transitions)->to;
						}
						else
							tempNode = (*transitions)->from;
						fileTrans<<(*subSp)->relatedState->id<<", "<<tempNode->id<<", "<<sqrt(CGAL::to_double(CGAL::squared_distance((*subSp)->relatedState->geometry,tempNode->geometry)))<<endl;
					}
				}
			//cout <<(*subSp)->relatedState->id<<", "<<CGAL::to_double((*subSp)->relatedState->geometry.x())
				//<<", "<<CGAL::to_double((*subSp)->relatedState->geometry.y())<<", "<<isDoor<<endl;
		}
		cout<<"count is "<<count<<"and abstract space is"<< myRoute->_nodes.size();
	}

	fileNodes.close();
	fileTrans.close();
}
