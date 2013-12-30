#include <string>
#include <vector>
#include <exception>
using namespace std;

#include "State.h"
#include "Transition.h"
#include "AbstractSpace.h"

int State::idS = 0;
State::State(void){
	isForNavigation = true;
	State::id = idS++;
}