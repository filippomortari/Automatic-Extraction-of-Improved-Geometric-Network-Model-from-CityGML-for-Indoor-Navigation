#include "node.h"


node::node(void)
{
	isDoor = false;
	doorName = "void";
}


node::~node(void)
{
}

int node::getId(void){
	return id;
}
void node::setId(int myid){
	id=myid;
}
Point_3 node::getGeometry(void){
	return geometry;
}
void node::setGeometry(Point_3 mygeom){
	geometry = mygeom;
}
bool node::getIsDoor(void){
	return isDoor;
}
void node::setIsDoor(bool val){
	isDoor = val;
}
std::string node::getDoorName(){
	return doorName;
}
void node::setDoorName(std::string num){
	doorName = num;
}