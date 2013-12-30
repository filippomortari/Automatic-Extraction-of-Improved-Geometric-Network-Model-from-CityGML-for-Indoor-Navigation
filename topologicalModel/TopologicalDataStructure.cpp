#include "TopologicalDataStructure.h"
#include "../config.h"

TopologicalDataStructure::TopologicalDataStructure(void)
{
}


TopologicalDataStructure::~TopologicalDataStructure(void)
{
}

void TopologicalDataStructure::addNode(node n){

	nodes[n.getId()] = n;

	if (!Config::debug)
		std::cout<<"node added. Array size: "<<nodes.size()<<endl<<"node id: "<<n.getId()<<"verify : "<<nodes[n.getId()].getId()<<endl;
}

void TopologicalDataStructure::addEdge(Edge e){
	edges[e.getID()] = e;
	if (!Config::debug)
		std::cout<<"edge added. Array size: "<<edges.size()<<endl;
}

void TopologicalDataStructure::addArea(Area a){
	areas[a.getId()] = a;
	if (!Config::debug)
		std::cout<<"Area added. Array size: "<<areas.size()<<endl;
}

map<int,Edge> TopologicalDataStructure::getEdges(void){
	return edges;
}
