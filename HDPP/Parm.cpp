#include <iostream>
#include <string>
#include "MbRandom.h"
#include "Model.h"
#include "Parm.h"



Parm::Parm(Model* mp, MenuItem* ip, long int initSeed, std::string pn) {

	initialSeed      = initSeed;
	modelPtr         = mp;
	parmName         = pn;
	assignedMenuItem = ip;
	activeParm       = 0;
	ranPtr           = new MbRandom(initSeed);
}

Parm::~Parm(void) {

	delete ranPtr;
}

int Parm::flip(int x) {

	if (x == 0)
		return 1;
	return 0;
}