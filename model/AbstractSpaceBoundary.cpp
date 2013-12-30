#include <string>
#include <vector>
#include <exception>
using namespace std;

#include "AbstractSpaceBoundary.h"
#include "Transition.h"

std::list<Point_2> AbstractSpaceBoundary::getGeometry(void){
return geometry;
}

bool AbstractSpaceBoundary::isNavigable(void){return false;}