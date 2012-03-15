#include <iomanip>
#include <iostream>
#include "Alignment.h"
#include "CondLikes.h"
#include "Msg.h"



CondLikes::CondLikes(Alignment* aln) {

	// initialize information on the dimensions
	numTaxa     = aln->getNumTaxa();
	numSites    = aln->getNumChar();
	numNodes    = 2 * numTaxa - 2;
	numStates   = 4;
	oneNodeSize = numSites * numStates;
	
	// initialize the conditional likelihoods
	initializeCondLikes(aln);
	
	//std::cout << "Initializing conditional likelihoods for alignment \"" << aln->getFileName() << "\"" << std::endl;
	print();
}

CondLikes::~CondLikes(void) {

	delete [] cls;
	delete [] clsPtr[0];
	delete [] clsPtr;
}

void CondLikes::initializeCondLikes(Alignment* aln) {

	// allocate and initialize an array holding the conditional probabilities
	cls = new double[numNodes * oneNodeSize];
	for (int i=0; i<numNodes*oneNodeSize; i++)
		cls[i] = 0.0;
	
	// allocate and initialize a 2-D array holding pointers to the cls array
	clsPtr = new double**[numNodes];
	clsPtr[0] = new double*[numNodes*numSites];
	for (int i=1; i<numNodes; i++)
		clsPtr[i] = clsPtr[i-1] + numSites;
	for (int i=0; i<numNodes; i++)
		for (int j=0; j<numSites; j++)
			clsPtr[i][j] = &cls[i*oneNodeSize + j*numStates];
			
	// initialize the conditional likelihoods that correspond to the observed tip states
	for (int i=0; i<numTaxa; i++)
		{
		for (int j=0; j<numSites; j++)
			{
			double *cl0 = clsPtr[i][j];
			double errorProbability = aln->getQualityScore(i, j);
			int numSetToOne = 0;
			int nucs[4];
			aln->getPossibleNucs(aln->getNucleotide(i, j), nucs);
			int numOnes = 0;
			for (int s=0; s<numStates; s++)	
				{
				cl0[s] = errorProbability / (numStates - 1);
				if (nucs[s] == 1)
					{
					cl0[s] = 1.0 - errorProbability;
					numOnes++;
					}
				}
			if (numOnes == 0)
				Msg::error("Problem initializing conditional likelihoods");
			}
		}
}

void CondLikes::print(void) {

	for (int j=0; j<numSites; j++)
		{
		std::cout << std::setw(4) << j << " -- ";
		for (int i=0; i<numTaxa; i++)
			{
			double *cl0 = clsPtr[i][j];
			for (int s=0; s<numStates; s++)
				std::cout << std::fixed << std::setprecision(0) << cl0[s];
			std::cout << " ";
			}
		std::cout << std::endl;
		}
}




