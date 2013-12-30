#include <string>
#include <vector>
#include <exception>
using namespace std;

#include "Transition.h"
#include "AbstractSpaceBoundary.h"
#include "State.h"

int Transition::idS = 0;
Transition::Transition(void){
	Transition::id =idS++;
}