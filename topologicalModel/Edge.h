#pragma once
#ifndef EDGE_H
#define EDGE_H
#include "node.h"
#include "Area.h"
#include "../config.h"

class Edge
{
public:
	int id;
	node* startNode;
	node* endNode;
	Area* leftArea;
	Area* rightArea;
	EdgeSemantics semantics;
	Edge* oppositeEdge;
	
	Edge(void);
	~Edge(void);
	void setID(int);
	void setStartNode(node*);
	void setEndNode(node*);
	void setLeftArea(Area*);
	void setRightArea(Area*);
	void setOppositeEdge(Edge*);

	int getID(void);
	node* getStartNode(void);
	node* getEndNode(void);
	Area* getLeftArea(void);
	Area* getRightArea(void);
	Edge* getOppositeEdge(void);
};


#endif

