#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <utility>
#include "Alignment.h"
#include "Franchise.h"
#include "Msg.h"
#include "Parm_length.h"
#include "Parm_subrates.h"
#include "Parm_statefreqs.h"
#include "Parm_tree.h"
#include "RateMatrix.h"
#include "RateMatrixMngr.h"



RateMatrixMngr::RateMatrixMngr(Model* mp, int ns) {

	modelPtr       = mp;
	numStates      = ns;
}

RateMatrixMngr::~RateMatrixMngr(void) {

	for (std::map<ParmKey, RateMatrix*>::iterator it = rateMatrixMap.begin(); it != rateMatrixMap.end(); it++)
		delete  it->second;
}

void RateMatrixMngr::addMatrixToMngr(ParmKey queryKey, RateMatrix* m) {

	rateMatrixMap.insert( std::make_pair(queryKey, m) );
}

void RateMatrixMngr::addMatrixToMngr(ParmTree* t, ParmLength* l, ParmSubRates* r, ParmStateFreqs* f, RateMatrix* m) {

	ParmKey queryKey(t, l, r, f);
	rateMatrixMap.insert( std::make_pair(queryKey, m) );
}

void RateMatrixMngr::deleteMatrix(RateMatrix* m) {

	delete m;
}

bool RateMatrixMngr::findDuplicateRateMatrices(std::set<RateMatrix*> &duplicates) {

	duplicates.clear();
	bool duplicatesPresent = false;
	for (std::map<ParmKey, RateMatrix*>::iterator it1 = rateMatrixMap.begin(); it1 != rateMatrixMap.end(); it1++)
		{
		ParmKey *key1 = it1->second->getParameterKey();
		for (std::map<ParmKey, RateMatrix*>::iterator it2 = it1; it2 != rateMatrixMap.end(); it2++)
			{
			if ( (*it1) == (*it2) )
				continue;
			ParmKey *key2 = it2->second->getParameterKey();
			if ( (*key1) == (*key2) )
				{
				duplicates.insert( it1->second );
				duplicates.insert( it2->second );
				duplicatesPresent = true;
				}
			
			}
		}
	return duplicatesPresent;
}

RateMatrix* RateMatrixMngr::getNewRateMatrix(ParmKey& k) {

	RateMatrix* m = new RateMatrix( k.getTree(), k.getStateFreqs(), k.getSubRates(), k.getLength(), numStates, this);
	return m;
}

RateMatrix* RateMatrixMngr::getNewRateMatrix(ParmTree* t, ParmLength* l, ParmSubRates* r, ParmStateFreqs* f) {

	RateMatrix* m = new RateMatrix(t, f, r, l, numStates, this);
	return m;
}

std::set<RateMatrix *>& RateMatrixMngr::getMatricesSharingLengthParm(Parm* p) {

	rateMatricesSharingParm.clear();
	for (std::map<ParmKey,RateMatrix*>::iterator it = rateMatrixMap.begin(); it != rateMatrixMap.end(); it++)
		{
		ParmKey key = it->first;
		if ( key.getLength() == p )
			rateMatricesSharingParm.insert( it->second );
		}
	return rateMatricesSharingParm;
}

std::set<RateMatrix *>& RateMatrixMngr::getMatricesSharingSubRatesParm(Parm* p) {

	rateMatricesSharingParm.clear();
	for (std::map<ParmKey,RateMatrix*>::iterator it = rateMatrixMap.begin(); it != rateMatrixMap.end(); it++)
		{
		ParmKey key = it->first;
		if ( key.getSubRates() == p )
			rateMatricesSharingParm.insert( it->second );
		}
	return rateMatricesSharingParm;
}

std::set<RateMatrix *>& RateMatrixMngr::getMatricesSharingStateFreqsParm(Parm* p) {

	rateMatricesSharingParm.clear();
	for (std::map<ParmKey,RateMatrix*>::iterator it = rateMatrixMap.begin(); it != rateMatrixMap.end(); it++)
		{
		ParmKey key = it->first;
		if ( key.getStateFreqs() == p )
			rateMatricesSharingParm.insert( it->second );
		}
	return rateMatricesSharingParm;
}

std::set<RateMatrix *>& RateMatrixMngr::getMatricesSharingTreeParm(Parm* p) {

	rateMatricesSharingParm.clear();
	for (std::map<ParmKey,RateMatrix*>::iterator it = rateMatrixMap.begin(); it != rateMatrixMap.end(); it++)
		{
		ParmKey key = it->first;
		if ( key.getTree() == p )
			rateMatricesSharingParm.insert( it->second );
		}
	return rateMatricesSharingParm;
}

std::set<RateMatrix*>& RateMatrixMngr::getMatricesSharingParm(Parm* p, int parmType) {

	if ( parmType == PARM_TREE )
		return getMatricesSharingTreeParm(p);
	else if ( parmType == PARM_LENG )
		return getMatricesSharingLengthParm(p);
	else if ( parmType == PARM_SUBR )
		return getMatricesSharingSubRatesParm(p);
	else if ( parmType == PARM_FREQ )
		return getMatricesSharingStateFreqsParm(p);
	else 
		Msg::error("Unknown parameter type");
	return getMatricesSharingTreeParm(p); // This return statement simply to remove a compile warning.
}

void RateMatrixMngr::print(void) {

	std::cout << "   Rate Matrix Manager:" << std::endl << std::endl;
	std::cout << "   ";
	std::cout << std::setw(5) << "ID";
	std::cout << std::setw(10) << "Tau";
	std::cout << std::setw(10) << "Pi";
	std::cout << std::setw(10) << "R";
	std::cout << std::setw(10) << "Length";
	std::cout << std::setw(10) << "Q";
	std::cout << std::endl;
	std::cout << "   ";
	for (int i=0; i<60; i++)
		std::cout << "-";
	std::cout << std::endl;
	
	int i = 0;
	for (std::map<ParmKey,RateMatrix*>::iterator it = rateMatrixMap.begin(); it != rateMatrixMap.end(); it++)
		{
		ParmKey key   = it->first;
		RateMatrix *m = it->second;
		std::cout << "   ";
		std::cout << std::setw(5) << i++;
		std::cout << std::setw(10) << key.getTree();
		std::cout << std::setw(10) << key.getStateFreqs();
		std::cout << std::setw(10) << key.getSubRates();
		std::cout << std::setw(10) << key.getLength();
		std::cout << std::setw(10) << m;
		std::cout << std::endl;
		}

	std::cout << "   ";
	for (int i=0; i<60; i++)
		std::cout << "-";
	std::cout << std::endl;
	std::cout << std::endl;
}

void RateMatrixMngr::removeMatrixFromMngr(RateMatrix* m) {

	rateMatrixMap.erase( *(m->getParameterKey()) );
	delete m;
}

RateMatrix* RateMatrixMngr::retrieveRateMatrix(ParmKey& queryKey) {

	std::map<ParmKey, RateMatrix*>::iterator it = rateMatrixMap.find( queryKey );
	if ( it != rateMatrixMap.end() )
		return it->second;
	return NULL;
}

RateMatrix* RateMatrixMngr::retrieveRateMatrix(ParmTree* t, ParmLength* l, ParmSubRates* r, ParmStateFreqs* f) {

	ParmKey queryKey(t, l, r, f);
	std::map<ParmKey,RateMatrix*>::iterator it = rateMatrixMap.find( queryKey );
	if ( it != rateMatrixMap.end() )
		return it->second;
	return NULL;
}


