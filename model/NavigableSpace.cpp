#include <string>
#include <vector>
#include <exception>
using namespace std;

#include "NavigableSpace.h"
#include "NonNavigableSpace.h"


void NavigableSpace::addNeighbour(NavigableSpace * space){
	_neighbours.push_back(space);
}

NavigableSpace::~NavigableSpace(void){
	for(int i=0; i<_subspaces.size(); ++i){
		delete _subspaces[i];

	}
	_subspaces.clear();
}

void NavigableSpace::foo(void){}