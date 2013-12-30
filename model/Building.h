#ifndef BUILDING_H
#define BUILDING_H

#include <string>
#include <vector>
#include <exception>
#include "AbstractSpace.h"
#include "AbstractSpaceBoundary.h"
#include "NavigableSpaceBoundary.h"
#include "GeneralSpace.h"
#include "TransitionSpace.h"
#include "Storey.h"
#include "Door.h"
#include "Elevator.h"
#include "Stairs.h"
#include "config.h"

using namespace std;


class AbstractSpace;
class Storey;
class Building;

class Building {
private:
public:
	std::vector<AbstractSpace*> buildingParts;
	std::vector<Storey> buildingStoreys;

	Building(void);
	~Building(void);
	void addStorey(int, string, double);
	GeneralSpace* addGeneralSpace(void);
	TransitionSpace* addTransitionSpace(void);
	Door* addDoorSpace(void);
	VerticalTransitionSpace* addVerticalTransitionSpace(SpaceSemantics);
};

#endif
