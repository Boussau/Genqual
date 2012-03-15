#include <iomanip>
#include <iostream>
#include <mpi.h>
#include "MbRandom.h"
#include "Mcmc.h"
#include "Model.h"
#include "SeedFactory.h"
#include "Settings.h"



Mcmc::Mcmc(Settings* sp, Model* mp) {

	// who am I?
	processId = MPI::COMM_WORLD.Get_rank();
	
	// remember the address of important objects
	modelPtr    = mp;
	settingsPtr = sp;
	
	// instantiate the random number generator
	SeedFactory& sf = SeedFactory::seedFactoryInstance();
	universalRanPtr = new MbRandom(sf.getUniversalSeed());
	
	// initialize the chain
	initializeChain();
	
	// run the chain
	runChain();
	
	// finalize (clean up) the chain
	finalizeChain();
}

Mcmc::~Mcmc(void) {

	delete universalRanPtr;
}

void Mcmc::finalizeChain(void) {

	if (processId == 0)
		{
		std::cout << std::endl;
		}
}

void Mcmc::initializeChain(void) {

	chainLength     = settingsPtr->getChainLength();
	printFrequency  = settingsPtr->getPrintFrequency();
	sampleFrequency = settingsPtr->getSampleFrequency();

	if (processId == 0)
		{
		std::cout << "   Markov chain Monte Carlo analysis:"                                               << std::endl;
		std::cout << "   * Number of MCMC cycles                 = " << chainLength                        << std::endl;
		std::cout << "   * Print-to-screen frequency             = " << printFrequency                     << std::endl;
		std::cout << "   * Sample frequency                      = " << sampleFrequency                    << std::endl;
		}
}

void Mcmc::printToScreen(int n, int franchiseToUpdate) {

	if (n % printFrequency == 0)
		{
		double lnL = modelPtr->calculateGlobalLikelihood();
		if (processId == 0)
			{
			char temp[50];
			sprintf(temp, "     - Cycle %d", n);
			std::string str = temp;
			std::cout << str;
			for (int i=0; i<43-str.size(); i++)
				std::cout << " ";
			std::cout << "= " << std::fixed << std::setprecision(3) << lnL;
			std::cout << " (Update to " << modelPtr->getFranchiseName(franchiseToUpdate) << ")" << std::endl;
			}
		}
}

void Mcmc::runChain(void) {

	MPI::COMM_WORLD.Barrier();
	double startT = MPI::Wtime();
	for (int n=1; n<=chainLength; n++)
		{
		// pick a franchise to update
		int franchiseToUpdate = modelPtr->pickFranchise();
		
		// update the franchise
		modelPtr->updateFranchise(franchiseToUpdate);
				
		// print information to the screen
		printToScreen(n, franchiseToUpdate);
		
		// sample the chain
		sampleChain(n);
		
		MPI::COMM_WORLD.Barrier();
		}
	MPI::COMM_WORLD.Barrier();
	double endT = MPI::Wtime();
	double totalT = endT - startT;
	if (processId == 0)
		std::cout << "   * Chain completed in " << totalT << " seconds" << std::endl;
}

void Mcmc::sampleChain(int n) {

	if (n % sampleFrequency == 0)
		modelPtr->sampleStates(n);
}

