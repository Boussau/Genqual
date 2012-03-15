#include <iostream>
#include <mpi.h>
#include "MbRandom.h"
#include "SeedFactory.h"

#undef REPEATABLE


SeedFactory::SeedFactory(void) {

	// who am I?
	processId = MPI::COMM_WORLD.Get_rank();

	// instantiate & initialize local (processor-specific) random number generator
#	ifndef REPEATABLE
	long int sd = (long int)time(NULL) + processId * 10001;
#	else
	long int sd = 1 + processId * 10001;
#	endif
	localSeedGenerator = new MbRandom(sd);

	// instantiate & initialize universal (shared) random number generator
	if (processId == 0)
		{
#		ifndef REPEATABLE
		sd = (long int)time(NULL);
#		else
		sd = 2;
#		endif
		}
	MPI::COMM_WORLD.Bcast(&sd, 1, MPI::LONG, 0);
	universalSeedGenerator = new MbRandom(sd);
	MPI::COMM_WORLD.Barrier();
}

SeedFactory::~SeedFactory(void) {

	delete localSeedGenerator;
	delete universalSeedGenerator;
}

long int SeedFactory::getLocalSeed(void) {

	localSeedGenerator->uniformRv();
	return localSeedGenerator->getSeed();
}

long int SeedFactory::getUniversalSeed(void) {

	universalSeedGenerator->uniformRv();
	return universalSeedGenerator->getSeed();
}

