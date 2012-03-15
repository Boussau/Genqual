#include <iostream>
#include <iomanip>
#include <string>
#include "MbRandom.h"
#include "Parm.h"
#include "Parm_length.h"



Length::Length(MbRandom *rp, int nt, double lam) {

	alpha = 2*nt-3;
	beta = lam;
	ranPtr = rp;
	setLengthFromPrior();
}

Length::Length(Length &k) {

	clone(k);
}

Length::~Length(void) {

}

Length& Length::operator=(const Length &k) {

	if (this != &k)
		clone(k);
	return *this;
}

void Length::clone(const Length &k) {

	alpha  = k.alpha;
	beta   = k.beta;
	ranPtr = k.ranPtr;
	length = k.length;
}

double Length::lnPriorProb(void) {

	return ranPtr->lnGammaPdf(alpha, beta, length);
	return 0.0;
}

void Length::print(void) {

	std::cout << std::fixed << std::setprecision(5) << length << std::endl;
}

void Length::setLengthFromPrior(void) {

	length = ranPtr->gammaRv(alpha, beta);
}

double Length::update(void) {

	double tuning = log(4.0);
	double oldVal = length;
	double newVal = oldVal * exp( tuning*(ranPtr->uniformRv()-0.5) );
	length = newVal;
	return log(newVal) - log(oldVal);
}

ParmLength::ParmLength(Model *mp, MenuItem *ip, long int initSeed, std::string pn, int nt, double lam) : Parm(mp, ip, initSeed, pn) {

	len[0] = new Length(ranPtr, nt, lam);
	len[1] = new Length( *len[0] );
}

ParmLength::~ParmLength(void) {

	delete len[0];
	delete len[1];
}

double ParmLength::drawFromPrior(void) {

	getActiveLength()->setLengthFromPrior();
	return 0.0;
}

void ParmLength::getParmString(std::string& s) {

	char temp[50];
	sprintf(temp, "%1.3lf", getActiveLength()->getVal());
	s = temp;
}

void ParmLength::keepUpdate(void) {

	*len[ flip(activeParm) ] = *len[ activeParm ];
}

double ParmLength::lnPriorProb(void) {

	return len[activeParm]->lnPriorProb();
}

void ParmLength::print(void) {

	len[activeParm]->print();
}

void ParmLength::restore(void) {

	*len[activeParm] = *len[ flip(activeParm) ];
}

double ParmLength::update(void) {

	return len[activeParm]->update();
}
