#include <string>
#include <vector>
#include <exception>
using namespace std;

#include "AbstractSpace.h"
#include "State.h"
#include "AbstractSpaceBoundary.h"
#include "Storey.h"

AbstractSpace::AbstractSpace(void){
	id = 0;
	relatedState = NULL;
}
AbstractSpace::~AbstractSpace(void){
	for(int i = 0; i<_boundedBy.size(); ++i){
		delete _boundedBy[i];

	}
	_boundedBy.clear();
}

