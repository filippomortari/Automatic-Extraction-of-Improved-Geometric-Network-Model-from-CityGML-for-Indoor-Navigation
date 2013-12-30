#pragma once



class Config
{
public:
	static const bool debug = true;
	 
	Config(void);
	~Config(void);
};

enum EdgeSemantics {WALL, DOOR};

enum SpaceSemantics {GENERALSPACE, ANCHORSPACE, WINDOWSPACE, DOORSPACE, TRANSITIONSPACE, STAIRSPACE, ELEVATORSPACE, ESCALATORSPACE, RAMPSPACE};
