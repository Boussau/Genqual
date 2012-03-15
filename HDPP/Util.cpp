#include <ctime>
#include "Util.h"


void Util::delay(double t) {

	clock_t startTick( clock() );
	while ( clock() - startTick < CLOCKS_PER_SEC * t )
		;
}

