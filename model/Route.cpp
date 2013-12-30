#include <string>
#include <vector>
#include <exception>
using namespace std;

#include "Route.h"
#include "RouteSegment.h"
#include "RouteNode.h"


Route::Route(void){}

Route::~Route(void){
	for(int i=0;i<_nodes.size();++i){
		delete _nodes[i];
	}

	for(int i=0;i<_edges.size();++i){
		delete _edges[i];
	}

	_edges.clear();
	_nodes.clear();
}
