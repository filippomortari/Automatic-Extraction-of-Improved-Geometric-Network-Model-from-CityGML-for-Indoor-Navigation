#include "EnvironmentParameters.h"


EnvironmentParameters* EnvironmentParameters::instance = 0;

EnvironmentParameters::EnvironmentParameters(void){
	//0.000012 0.000050 ok // a 45 prende anche la stanza 472
	/*
	TABLE OF PARAMETERS FOR OSM OTB BUILDING
	lOffset = 0.000045;
	samplingRate = 0.00002; //0.00002
	precision = 0.00000113;

	-----------------------------------------
	TABLE OF PARAMETERS FOR CITYGML
	lOffset = 0.45;
	samplingRate = 0.70; //0.00002
	precision = 0.00000113;

	*/
	lOffset = 0.000045;
	samplingRate = 0.00008; //0.00002
	precision = 0.00000113;
}

EnvironmentParameters* EnvironmentParameters::getInstance ()
{
    if (instance == 0)
        instance = new EnvironmentParameters;
    return instance;
}
