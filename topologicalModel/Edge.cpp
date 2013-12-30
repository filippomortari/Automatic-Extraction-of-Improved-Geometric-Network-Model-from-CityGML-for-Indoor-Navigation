#include "Edge.h"


Edge::Edge(void)
{
	rightArea = NULL;
	leftArea = NULL;
	oppositeEdge = NULL;
}


Edge::~Edge(void)
{
}

void Edge::setID(int i){
	id = i;
}
int Edge::getID(void){
	return id;
}
void Edge::setStartNode(node* nodo){
	startNode = nodo;
}

void Edge::setEndNode(node* nodo){
	endNode = nodo;
}

void Edge::setLeftArea(Area* a){
	leftArea = a;
}

void Edge::setRightArea(Area* a){
	rightArea = a;
}

void Edge::setOppositeEdge(Edge* e){
	oppositeEdge = e;
}

node* Edge::getStartNode(void){
	return startNode;
}

node* Edge::getEndNode(void){
	return endNode;
}

Area* Edge::getLeftArea(void){
	return leftArea;
}

Area* Edge::getRightArea(void){
	return rightArea;
}

Edge* Edge::getOppositeEdge(void){
	return oppositeEdge;
}

