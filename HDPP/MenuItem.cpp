#include "Alignment.h"
#include <iostream>
#include "Franchise.h"
#include "MenuItem.h"
#include "Model.h"
#include "Msg.h"
#include "Parm.h"
#include "Parm_length.h"
#include "Parm_subrates.h"
#include "Parm_statefreqs.h"
#include "Parm_tree.h"
#include "SeedFactory.h"
#include "Settings.h"
#include "TableImage.h"
#include "TableImageFactory.h"



MenuItem::MenuItem(MbRandom* rp, Model* mp, Settings* sp, int pt, std::vector<std::string> tn) {

	// remember the address of important objects
	ranPtr      = rp;
	modelPtr    = mp;
	settingsPtr = sp;
	
	// what type of parameter am I?
	parmType  = pt;
	parameter = NULL;
	
	// instantiate the parameter
	SeedFactory& sf = SeedFactory::seedFactoryInstance();
	long int initSeed = sf.getLocalSeed();
	initializeMenuItem(tn, initSeed);
}

MenuItem::MenuItem(MbRandom* rp, Model* mp, Settings* sp, int pt, std::vector<std::string> tn, long int initSeed) {

	// remember the address of important objects
	ranPtr      = rp;
	modelPtr    = mp;
	settingsPtr = sp;
	
	// what type of parameter am I?
	parmType  = pt;
	parameter = NULL;
	
	// instantiate the parameter
	initializeMenuItem(tn, initSeed);
}

MenuItem::~MenuItem(void) {

	clearTableImages();
	if (parameter != NULL)
		delete parameter;
}

void MenuItem::addTableImage(TableImage* t) {

	t->assignToMenuItem(index);
	tableImages.insert( t );
}

void MenuItem::clearTableImages(void) {

	TableImageFactory& tif = TableImageFactory::tableImageFactoryInstance(); 
	for (std::set<TableImage*>::iterator t=tableImages.begin(); t != tableImages.end(); t++)
		tif.returnTableImageToPool( (*t) );
}

TableImage* MenuItem::findTableImage(int& rIdx, int& tIdx) {

	for (std::set<TableImage*>::iterator t=tableImages.begin(); t != tableImages.end(); t++)
		{
		if ( (*t)->getRestaurantIndex() == rIdx && (*t)->getTableIndex() == tIdx )
			return (*t);
		}
	return NULL;
}

int MenuItem::getInitialParameterSeed(void) {

	return parameter->getInitialSeed();
}

std::string MenuItem::getParameterString(void) {

	std::string pStr = "";
	parameter->getParmString(pStr);
	return pStr;
}

void MenuItem::initializeMenuItem(std::vector<std::string> tn, long int initSeed) {
	
	if ( parmType == PARM_TREE )
		{
		parameter = new ParmTree(modelPtr, this, initSeed, "Tree", tn, settingsPtr->getBrlenLambda());
		if (!parameter)
			Msg::error("Cannot allocate tree parameter");
		}
	else if ( parmType == PARM_LENG )
		{
		parameter = new ParmLength(modelPtr, this, initSeed, "Length", tn.size(), settingsPtr->getBrlenLambda());
		if (!parameter)
			Msg::error("Cannot allocate tree-length parameter");
		}
	else if ( parmType == PARM_SUBR )
		{
		parameter = new ParmSubRates(modelPtr, this, initSeed, "Exch. Parms.", 6.0);
		if (!parameter)
			Msg::error("Cannot allocate GTR parameter");
		}
	else if ( parmType == PARM_FREQ )
		{
		parameter = new ParmStateFreqs(modelPtr, this, initSeed, "Nucleotide Frequencies", modelPtr->getNumStates(), modelPtr->getNumStates());
		if (!parameter)
			Msg::error("Cannot allocate Pi parameter");
		}
	else 
		{
		std::cout << "parmType = " << parmType << std::endl;
		Msg::error("Cannot determine parameter type");
		}
}

bool MenuItem::isTableAssignedToMenuItem(int rIdx, int tIdx) {

	for (std::set<TableImage*>::iterator t=tableImages.begin(); t != tableImages.end(); t++)
		{
		if ( (*t)->getRestaurantIndex() == rIdx && (*t)->getTableIndex() == tIdx )
			return true;
		}
	return false;
}

void MenuItem::keepUpdate(void) {

	parameter->keepUpdate();
}

double MenuItem::lnPriorProbability(void) {

	return parameter->lnPriorProb();
}

void MenuItem::print(void) {

	std::cout << "Menu item indexed " << getIndex() << " with " << getNumTableImages() << " table images and stick length " << stickLength << std::endl;
	parameter->print();
}

void MenuItem::removeAllTableImages(void) {

	TableImageFactory& tif = TableImageFactory::tableImageFactoryInstance(); 
	for (std::set<TableImage*>::iterator t=tableImages.begin(); t != tableImages.end(); t++)
		tif.returnTableImageToPool(*t);
	tableImages.clear();
}

void MenuItem::removeTableImage(TableImage* t) {

	TableImageFactory& tif = TableImageFactory::tableImageFactoryInstance(); 
	tif.returnTableImageToPool(t);
	tableImages.erase(t);
}

void MenuItem::removeTableImage(int rIdx, int tIdx) {

	TableImage* t = findTableImage(rIdx, tIdx);
	removeTableImage(t);
}

void MenuItem::restore(void) {

	parameter->restore();
}

double MenuItem::updateParameter(void) {

	double lnX = 0.0;
	if ( getNumTableImages() > 0 )
		lnX = parameter->update();
	else 
		lnX = parameter->drawFromPrior();
	return lnX;
}

