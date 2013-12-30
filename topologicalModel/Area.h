#pragma once
#ifndef AREA_H
#define AREA_H
#include <list>

class Edge;

class Area{

public:
	int id;
	std::list<Edge*> edges;

	Area(void);
	~Area(void);
	void setId(int);
	int getId(void);
	void addEdge(Edge*);
	std::list<Edge*> getEdges(void);
};


#endif