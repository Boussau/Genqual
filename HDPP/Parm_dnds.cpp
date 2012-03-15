#include <iostream>
#include <iomanip>
#include <string>
#include "MbRandom.h"
#include "Parm.h"
#include "Parm_dnds.h"



Dnds::Dnds(MbRandom *rp) {

	ranPtr = rp;
	setDndsFromPrior();
}

Dnds::Dnds(Dnds &d) {

	clone(d);
}

Dnds::~Dnds(void) {

}

Dnds& Dnds::operator=(const Dnds &d) {

	if (this != &d)
		clone(d);
	return *this;
}

void Dnds::clone(const Dnds &d) {

	ranPtr = d.ranPtr;
	dnds = d.dnds;
}

double Dnds::lnPriorProb(void) {

	return -2.0 * log(dnds + 1.0);
}

void Dnds::print(void) {

	std::cout << std::fixed << std::setprecision(5) << dnds << std::endl;
}

void Dnds::setDndsFromPrior(void) {

	dnds = ranPtr->exponentialRv(1.0) / ranPtr->exponentialRv(1.0);
}

double Dnds::update(void) {

	double tuning = log(2.0);
	double oldVal = dnds;
	double newVal = oldVal * exp( tuning*(ranPtr->uniformRv()-0.5) );
	dnds = newVal;
	return log(newVal) - log(oldVal);
}

ParmDnds::ParmDnds(Model *mp, MenuItem *ip, long int initSeed, std::string pn) : Parm(mp, ip, initSeed, pn) {

	dnds[0] = new Dnds(ranPtr);
	dnds[1] = new Dnds( *dnds[0] );
}

ParmDnds::~ParmDnds(void) {

	delete dnds[0];
	delete dnds[1];
}

double ParmDnds::drawFromPrior(void) {

	getActiveDnds()->setDndsFromPrior();
	return 0.0;
}

void ParmDnds::getParmString(std::string& s) {

	char temp[50];
	sprintf(temp, "%1.3lf", getActiveDnds()->getVal());
	s = temp;
}

void ParmDnds::keepUpdate(void) {

	*dnds[ flip(activeParm) ] = *dnds[ activeParm ];
}

double ParmDnds::lnPriorProb(void) {

	return dnds[activeParm]->lnPriorProb();
}

void ParmDnds::print(void) {

	dnds[activeParm]->print();
}

void ParmDnds::restore(void) {

	*dnds[ activeParm ] = *dnds[ flip(activeParm) ];
}

double ParmDnds::update(void) {

	return dnds[activeParm]->update();
}

