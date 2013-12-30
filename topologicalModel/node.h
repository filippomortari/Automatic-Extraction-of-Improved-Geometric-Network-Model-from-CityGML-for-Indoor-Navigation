#pragma once
#include "../CGALDefines.h"


class node
{

	
public:
	int id;
	Point_3 geometry;
	bool isDoor;
	std::string doorName;
	
	node(void);
	~node(void);

	int getId(void);
	void setId(int);
	Point_3 getGeometry(void);
	void setGeometry(Point_3);
	bool getIsDoor(void);
	void setIsDoor(bool);
	std::string getDoorName();
	void setDoorName(std::string);

};

