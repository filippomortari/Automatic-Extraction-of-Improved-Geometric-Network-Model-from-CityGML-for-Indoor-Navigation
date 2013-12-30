#include "Area.h"


Area::Area(void)
{
}


Area::~Area(void)
{
}

void Area::setId(int i){
	id = i;
}

int Area::getId(void) {
	return id;
}

void Area::addEdge(Edge* edge){
	edges.push_back(edge);
}

std::list<Edge*> Area::getEdges(void){
	return edges;
}
