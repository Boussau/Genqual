#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "MbRandom.h"
#include "Parm.h"
#include "Parm_subrates.h"



SubRates::SubRates(MbRandom *rp, double a) {

	ranPtr = rp;
	alpha = a;
	rates.resize(6);
	dirParm.resize(6);
	for (int i=0; i<6; i++)
		dirParm[i] = alpha / 6.0;
	setSubRatesFromPrior();
}

SubRates::SubRates(SubRates &f) {

	alpha = 0.0;
	clone(f);
}

SubRates::~SubRates(void) {

}

SubRates& SubRates::operator=(const SubRates &f) {

	if (this != &f)
		clone(f);
	return *this;
}

void SubRates::clone(const SubRates &f) {

	ranPtr = f.ranPtr;
	alpha = f.alpha;
	if (rates.size() != 6)
		rates.resize(6);
	if (dirParm.size() != 6)
		dirParm.resize(6);
	for (int i=0; i<6; i++)
		{
		rates[i] = f.rates[i];
		dirParm[i] = f.dirParm[i];
		}
}

double SubRates::lnPriorProb(void) {

	return ranPtr->lnDirichletPdf(dirParm, rates);
}
 
double SubRates::normalizeRates(std::vector<double> &a, double minVal, double total) {

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

void SubRates::print(void) {

	std::cout << "(";
	for (int i=0; i<6; i++)
		{
		std::cout << std::fixed << std::setprecision(5) << rates[i];
		if (i + 1 != 6)
			std::cout << ",";
		}
	std::cout << ")" << std::endl;
}

void SubRates::setSubRatesFromPrior(void) {

	ranPtr->dirichletRv(dirParm, rates);
}

double SubRates::update(void) {

	double alpha0 = 400.0;
	std::vector<double> aForward(6);
	std::vector<double> aReverse(6);
	std::vector<double> oldRates(6);
	for (int i=0; i<6; i++)
		{
		oldRates[i] = rates[i];
		aForward[i] = rates[i] * alpha0;
		}
	ranPtr->dirichletRv(aForward, rates);
	for (int i=0; i<6; i++)
		aReverse[i] = rates[i] * alpha0;
	return ranPtr->lnDirichletPdf(aReverse, oldRates) - ranPtr->lnDirichletPdf(aForward, rates);
}

ParmSubRates::ParmSubRates(Model* mp, MenuItem* ip, long int initSeed, std::string pn, double a) : Parm(mp, ip, initSeed, pn) {

	sr[0] = new SubRates(ranPtr, a);
	sr[1] = new SubRates( *sr[0] );
}

ParmSubRates::~ParmSubRates(void) {

	delete sr[0];
	delete sr[1];
}

double ParmSubRates::drawFromPrior(void) {

	getActiveSubRates()->setSubRatesFromPrior();
	return 0.0;
}

void ParmSubRates::getParmString(std::string& s) {

	std::vector<double>& f = getActiveSubRates()->getVal();

	for (int i=0; i<f.size(); i++)
		{
		char temp[50];
		sprintf(temp, "%1.3lf", f[i]);
		s += temp;
		if (i + 1 < f.size())
			s += '\t';
		}
}

void ParmSubRates::keepUpdate(void) {

	*sr[ flip(activeParm) ] = *sr[ activeParm ];
}

double ParmSubRates::lnPriorProb(void) {

	return sr[activeParm]->lnPriorProb();
}

void ParmSubRates::print(void) {

	sr[activeParm]->print();
}

void ParmSubRates::restore(void) {

	*sr[activeParm] = *sr[ flip(activeParm) ];
}

double ParmSubRates::update(void) {

	return sr[activeParm]->update();
}
