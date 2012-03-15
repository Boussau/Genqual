#include <iostream>
#include <iomanip>
#include <string>
#include "MbRandom.h"
#include "Parm.h"
#include "Parm_kappa.h"



Kappa::Kappa(MbRandom *rp) {

	ranPtr = rp;
	setKappaFromPrior();
}

Kappa::Kappa(Kappa &k) {

	clone(k);
}

Kappa::~Kappa(void) {

}

Kappa& Kappa::operator=(const Kappa &k) {

	if (this != &k)
		clone(k);
	return *this;
}

void Kappa::clone(const Kappa &k) {

	ranPtr = k.ranPtr;
	kappa = k.kappa;
}

double Kappa::lnPriorProb(void) {

	return -2.0 * log(kappa + 1.0);
}

void Kappa::print(void) {

	std::cout << std::fixed << std::setprecision(5) << kappa << std::endl;
}

void Kappa::setKappaFromPrior(void) {

	kappa = ranPtr->exponentialRv(1.0) / ranPtr->exponentialRv(1.0);
}

double Kappa::update(void) {

	double tuning = log(4.0);
	double oldVal = kappa;
	double newVal = oldVal * exp( tuning*(ranPtr->uniformRv()-0.5) );
	kappa = newVal;
	return log(newVal) - log(oldVal);
}

ParmKappa::ParmKappa(Model *mp, MenuItem *ip, long int initSeed, std::string pn) : Parm(mp, ip, initSeed, pn) {

	kappa[0] = new Kappa(ranPtr);
	kappa[1] = new Kappa( *kappa[0] );
}

ParmKappa::~ParmKappa(void) {

	delete kappa[0];
	delete kappa[1];
}

double ParmKappa::drawFromPrior(void) {

	getActiveKappa()->setKappaFromPrior();
	return 0.0;
}

void ParmKappa::getParmString(std::string& s) {

	char temp[50];
	sprintf(temp, "%1.3lf", getActiveKappa()->getVal());
	s = temp;
}

void ParmKappa::keepUpdate(void) {

	*kappa[ flip(activeParm) ] = *kappa[ activeParm ];
}

double ParmKappa::lnPriorProb(void) {

	return kappa[activeParm]->lnPriorProb();
}

void ParmKappa::print(void) {

	kappa[activeParm]->print();
}

void ParmKappa::restore(void) {

	*kappa[activeParm] = *kappa[ flip(activeParm) ];
}

double ParmKappa::update(void) {

	return kappa[activeParm]->update();
}
