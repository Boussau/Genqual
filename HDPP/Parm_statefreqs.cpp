#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "MbRandom.h"
#include "Parm.h"
#include "Parm_statefreqs.h"



StateFreqs::StateFreqs(MbRandom *rp, int n, double a) {

	ranPtr    = rp;
	numStates = n;
	alpha     = a;
	freqs.resize(numStates);
	dirParm.resize(numStates);
	for (int i=0; i<numStates; i++)
		dirParm[i] = alpha / numStates;
	setStateFreqsFromPrior();
}

StateFreqs::StateFreqs(StateFreqs &f) {

	numStates = 0;
	alpha     = 0.0;
	clone(f);
}

StateFreqs::~StateFreqs(void) {

}

StateFreqs& StateFreqs::operator=(const StateFreqs &f) {

	if (this != &f)
		clone(f);
	return *this;
}

void StateFreqs::clone(const StateFreqs &f) {

	ranPtr = f.ranPtr;
	if (numStates != f.numStates)
		{
		freqs.resize(f.numStates);
		dirParm.resize(f.numStates);
		}
	numStates = f.numStates;
	alpha = f.alpha;
	for (int i=0; i<numStates; i++)
		{
		freqs[i] = f.freqs[i];
		dirParm[i] = f.dirParm[i];
		}
}

double StateFreqs::lnPriorProb(void) {

	return ranPtr->lnDirichletPdf(dirParm, freqs);
}
 
double StateFreqs::normalizeFreqs(std::vector<double> &a, double minVal, double total) {

	int n = a.size();
	double normalizeTo = total;
	double sum = 0.0;
	for (int i=0; i<n; i++)
		{
		if (a[i] <= minVal)
			normalizeTo -= minVal;
		else
			sum += a[i];
		}
	for (int i=0; i<n; i++)
		{
		if (a[i] <= minVal)
			a[i] = minVal;
		else
			a[i] *= (normalizeTo/sum);
		}
	sum = 0.0;
	for (int i=0; i<n; i++)
		sum += a[i];
	return sum;
}

void StateFreqs::print(void) {

	std::cout << "(";
	for (int i=0; i<numStates; i++)
		{
		std::cout << std::fixed << std::setprecision(5) << freqs[i];
		if (i + 1 != numStates)
			std::cout << ",";
		}
	std::cout << ")" << std::endl;
}

void StateFreqs::setStateFreqsFromPrior(void) {

	ranPtr->dirichletRv(dirParm, freqs);
}

double StateFreqs::update(void) {

	double alpha0 = 300.0;

	std::vector<double> aForward(4);
	std::vector<double> aReverse(4);
	std::vector<double> oldFreqs(4);
	for (int i=0; i<4; i++)
		{
		oldFreqs[i] = freqs[i];
		aForward[i] = freqs[i] * alpha0;
		}
	ranPtr->dirichletRv(aForward, freqs);
	for (int i=0; i<4; i++)
		aReverse[i] = freqs[i] * alpha0;
	return ranPtr->lnDirichletPdf(aReverse, oldFreqs) - ranPtr->lnDirichletPdf(aForward, freqs);
}

ParmStateFreqs::ParmStateFreqs(Model* mp, MenuItem* ip, long int initSeed, std::string pn, int n, double a) : Parm(mp, ip, initSeed, pn) {

	sf[0] = new StateFreqs(ranPtr, n, a);
	sf[1] = new StateFreqs( *sf[0] );
}

ParmStateFreqs::~ParmStateFreqs(void) {

	delete sf[0];
	delete sf[1];
}

double ParmStateFreqs::drawFromPrior(void) {

	getActiveStateFreqs()->setStateFreqsFromPrior();
	return 0.0;
}

void ParmStateFreqs::getParmString(std::string& s) {

	std::vector<double>& f = getActiveStateFreqs()->getVal();

	for (int i=0; i<f.size(); i++)
		{
		char temp[50];
		sprintf(temp, "%1.3lf", f[i]);
		s += temp;
		if (i + 1 < f.size())
			s += '\t';
		}
}

void ParmStateFreqs::keepUpdate(void) {

	*sf[ flip(activeParm) ] = *sf[ activeParm ];
}

double ParmStateFreqs::lnPriorProb(void) {

	return sf[activeParm]->lnPriorProb();
}

void ParmStateFreqs::print(void) {

	sf[activeParm]->print();
}

void ParmStateFreqs::restore(void) {

	*sf[activeParm] = *sf[ flip(activeParm) ];
}

double ParmStateFreqs::update(void) {

	return sf[activeParm]->update();
}
