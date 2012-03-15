#include <iostream>
#include <iomanip>
#include "MbTransitionMatrix.h"
#include "MbMatrix.h"
#include "Parm.h"
#include "Parm_length.h"
#include "Parm_subrates.h"
#include "Parm_key.h"
#include "Parm_statefreqs.h"
#include "Parm_tree.h"
#include "RateMatrix.h"
#include "RateMatrixMngr.h"



RateMatrix::RateMatrix(ParmTree* t, ParmStateFreqs* f, ParmSubRates* r, ParmLength* l, int n, RateMatrixMngr* mp) {

	// initialize variables
	treePtr                = t;
	stateFreqsPtr          = f;
	srPtr                  = r;
	lenPtr                 = l;
	numStates              = n;
	activeQ                = 0;
	activeTi               = 0;
	rateMatrixMngrPtr      = mp;
	numNodes               = treePtr->getActiveTree()->getNumNodes();
	
	// allocate parameter key (each rate matrix keeps a key to its parameters pointers)
	parameterKey = new ParmKey( t, l, r, f );
	
	// allocate rate matrix
	q[0] = MbMatrix<double>(numStates, numStates, 0.0);
	q[1] = MbMatrix<double>(numStates, numStates, 0.0);
	
	// set the rate matrices
	setRateMatrix();
	flipActiveQ();
	setRateMatrix();
	flipActiveQ();
			
	// instantiate the transition probability matrix calculator now
	allocateTransitionProbabilities();
		
	// set up the rate matrix
	updateRateMatrix();
	updateRateMatrix();
}

RateMatrix::~RateMatrix(void) {

	delete parameterKey;
	delete ti;
	delete [] tiProb[0];
	delete [] tiProb[1];
}

void RateMatrix::allocateTransitionProbabilities(void) {

	// instantiate the transition matrix calculator and transition matrices
	ti = new MbTransitionMatrix(q[0], true);
	tiProb[0] = new MbMatrix<double>[numNodes];
	for (int i=0; i<numNodes; i++)
		tiProb[0][i] = MbMatrix<double>(numStates, numStates);
	tiProb[1] = new MbMatrix<double>[numNodes];
	for (int i=0; i<numNodes; i++)
		tiProb[1][i] = MbMatrix<double>(numStates, numStates);
}

MbMatrix<double> &RateMatrix::getUniformizedRateMatrix(double* mu) {

	return ti->uniformize(mu);
}

void RateMatrix::print(void) {

	for (int i=0; i<numStates; i++)
		{
		for (int j=0; j<numStates; j++)
			{
			if (q[activeQ][i][j] < 0.0)
				std::cout << std::fixed << std::setprecision(4) << q[activeQ][i][j] << " ";
			else
				std::cout << std::fixed << std::setprecision(4) << " " << q[activeQ][i][j] << " ";
			}
		std::cout << std::endl;
		}
}

void RateMatrix::printTis(void) {

	for (int n=0; n<numNodes; n++)
		{
		Node *p = treePtr->getActiveTree()->getDownPassNode(n);
		if (p->getAnc() != NULL)
			{
			MbMatrix<double> P0 = getTiMatrix( 0, p->getIndex() );
			MbMatrix<double> P1 = getTiMatrix( 1, p->getIndex() );
			std::cout << "Transition Matrices for node " << p->getIndex() << " (" << activeTi << ")" << std::endl;
			for (int i=0; i<numStates; i++)
				{
				double sum = 0.0;
				for (int j=0; j<numStates; j++)
					{
					std::cout << std::fixed << std::setprecision(2) << P0[i][j] << " ";
					sum += P0[i][j];
					}
				std::cout << std::fixed << std::setprecision(6) << sum << " ";
				std::cout << "   ";
				sum = 0.0;
				for (int j=0; j<numStates; j++)
					{
					std::cout << std::fixed << std::setprecision(2) << P1[i][j] << " ";
					sum += P1[i][j];
					}
				std::cout << std::fixed << std::setprecision(6) << sum << " ";
				std::cout << std::endl;
				break;
				}
			}
		}
}

void RateMatrix::restoreRateMatrix(Parm* p) {

	ParmTree* t = dynamic_cast<ParmTree*>(p);
	if ( t == 0 )
		{
		flipActiveQ();
		ti->restoreQ();
		}
	flipActiveTi();
}

void RateMatrix::setParmPtrs(ParmTree* t, ParmStateFreqs* f, ParmSubRates* r, ParmLength* l) {

	parameterKey->setTree(t);
	treePtr = t; 

	parameterKey->setStateFreqs(f);
	stateFreqsPtr = f; 

	parameterKey->setSubRates(r);
	srPtr = r; 

	parameterKey->setLength(l);
	lenPtr = l; 
}

void RateMatrix::setSubRatesPtr(ParmSubRates* r) { 

	parameterKey->setSubRates(r);
	srPtr = r; 
}

void RateMatrix::setLengthPtr(ParmLength* l) { 

	parameterKey->setLength(l);
	lenPtr = l; 
}

void RateMatrix::setStateFreqsPtr(ParmStateFreqs* f) { 

	parameterKey->setStateFreqs(f);
	stateFreqsPtr = f; 
}

void RateMatrix::setTreePtr(ParmTree* t) { 

	parameterKey->setTree(t);
	treePtr = t; 
}

void RateMatrix::setRateMatrix(void) {

	// get the parameters
	std::vector<double> rates = srPtr->getActiveSubRates()->getVal();
	std::vector<double> freqs = stateFreqsPtr->getActiveStateFreqs()->getVal();
	
	// set the diagonal entries to zero
	for (int i=0; i<numStates; i++)
		q[activeQ][i][i] = 0.0;
	
	// enter the values
	double averageRate = 0.0;
	for (int i=0, k=0; i<numStates; i++)
		{
		for (int j=i+1; j<numStates; j++)
			{
			q[activeQ][i][j] = rates[k] * freqs[j];
			q[activeQ][j][i] = rates[k] * freqs[i];
			k++;
			
			averageRate += freqs[i] * q[activeQ][i][j];
			averageRate += freqs[j] * q[activeQ][j][i];
			}
		}
		
	// enter the diagonal elements
	for (int i=0; i<numStates; i++)
		{
		double sum = 0.0;
		for (int j=0; j<numStates; j++)
			{
			if (i != j)
				sum += q[activeQ][i][j];
			}
		q[activeQ][i][i] = -sum;
		}
		
	// rescale the rate matrix such that the mean rate of synonymous substitution is one
	double scalingFactor = (1.0 / averageRate);
	for (int i=0; i<numStates; i++)
		for (int j=0; j<numStates; j++)
			q[activeQ][i][j] *= scalingFactor;
		
#	if 0
	std::cout << "Q Matrix" << std::endl;
	for (int i=0; i<numStates; i++)
		{
		double sum = 0.0;
		for (int j=0; j<numStates; j++)
			{
			std::cout << std::fixed << std::setprecision(4);
			if (q[activeQ][i][j] < 0.0)
				std::cout << q[activeQ][i][j] << " ";
			else
				std::cout << " " << q[activeQ][i][j] << " ";
			sum += q[activeQ][i][j];
			}
		std::cout << " (" << sum << ")";
		std::cout << std::endl;
		}
#	endif
}

void RateMatrix::updateRateMatrix(void) {

	// set the rate matrix
	flipActiveQ();
	setRateMatrix();
		
	// update the eigen system and the transition probabilities
	ti->updateQ( q[activeQ] );
		
	// update the transition probabilities
	flipActiveTi();
	updateTransitionProbabilities();
}

void RateMatrix::updateRateMatrix(Parm* p) {

	ParmTree* t = dynamic_cast<ParmTree*>(p);
	if ( t == 0 )
		{
		// flip the active space
		flipActiveQ();
		
		// set the rate matrix
		setRateMatrix();
			
		// update the eigen system and the transition probabilities
		ti->updateQ( q[activeQ] );
		}
		
	// update the transition probabilities
	flipActiveTi();
	updateTransitionProbabilities();
}

void RateMatrix::updateTransitionProbabilities(void) {
		
	Tree* t = treePtr->getActiveTree();
	double treeLength = lenPtr->getActiveLength()->getVal();
	for (int i=0; i<numNodes; i++)
		{
		Node* p = t->getDownPassNode(i);
		if (p->getAnc() != NULL)
			{
			double v = p->getP() * treeLength;
			int idx = p->getIndex();
			ti->tiProbs( v, tiProb[activeTi][idx] );
			}
		}
}




