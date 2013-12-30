#include <string>
#include <vector>
#include <exception>
using namespace std;

#include "Building.h"
#include "AbstractSpace.h"
#include "Storey.h"
#include "GeneralSpace.h"
#include "TransitionSpace.h"

Building::Building(void){
}

Building::~Building(void){
	for(int i=0; i<buildingParts.size();++i){
		delete buildingParts[i];
	}
	buildingParts.clear();
}

void Building::addStorey(int i, string str, double elev){
	Storey s;
	s.storeyNumber = i;
	s.storeyName = str;
	s.elevation = elev;
	buildingStoreys.push_back(s);
}

GeneralSpace* Building::addGeneralSpace(void){
	buildingParts.push_back(new GeneralSpace());
	return (GeneralSpace*) buildingParts.back();
}

TransitionSpace* Building::addTransitionSpace(void){
	buildingParts.push_back(new TransitionSpace());
	return (TransitionSpace*) buildingParts.back();
}

Door* Building::addDoorSpace(void){
	buildingParts.push_back(new Door());
	return (Door*) buildingParts.back();
}

VerticalTransitionSpace* Building::addVerticalTransitionSpace(SpaceSemantics s) {
	if (s==STAIRSPACE){
		buildingParts.push_back(new Stairs());
	}
	else if (s==ELEVATORSPACE) {
		buildingParts.push_back(new Elevator());
	}

	return (VerticalTransitionSpace*) buildingParts.back();
}

