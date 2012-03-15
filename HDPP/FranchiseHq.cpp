#include <mpi.h>
#include <iomanip>
#include <iostream>
#include <vector>
#include "Franchise.h"
#include "FranchiseHq.h"
#include "MbRandom.h"
#include "MenuItem.h"
#include "MenuItemInfo.h"
#include "Msg.h"
#include "Parm_tree.h"
#include "RateMatrix.h"
#include "RateMatrixMngr.h"
#include "Restaurant.h"
#include "SeedFactory.h"
#include "Settings.h"
#include "SiteLikes.h"
#include "SiteModels.h"
#include "Table.h"
#include "TableImage.h"
#include "TableImageFactory.h"
#include "TableInfo.h"



FranchiseHq::FranchiseHq(Model* mp, Franchise* fp, Settings* sp, std::vector<std::string>& tn, std::vector<TableImage*>& tableImages) {

	// who am I?
	processId    = MPI::COMM_WORLD.Get_rank();
	numProcesses = MPI::COMM_WORLD.Get_size();
		
	// instantiate the random number generator for this object
	ranPtr = new MbRandom;
	
	// remember the locations of important objects
	franchisePtr = fp;
	modelPtr     = mp;
	settingsPtr  = sp;
	
	// initialize some important parameters
	menuAlpha             = settingsPtr->getMenuAlpha();
	taxonNames            = tn;
	numAuxiliaryMenuItems = settingsPtr->getNumAuxiliaryMenuItems();
	parmType              = franchisePtr->getParmType();
	parmTreatment         = franchisePtr->getParmTreatment();

	// initialize a vector holding the potential menu item indices
	for (int i=0; i<franchisePtr->getNumPatronsInFranchise(); i++)
		availableMenuItemIndices.insert( i );

	// initialize the franchise headquarters
	initializeFranchiseHq(tableImages);

	// open file for output
	openFile();
	
	//print();
}

FranchiseHq::FranchiseHq(Model* mp, Franchise* fp, Settings *sp, std::vector<std::string>& tn, std::vector<MenuItemInfo*>& mi, std::vector<TableInfo*>& ti, std::vector<double>& si) {

	// who am I?
	processId    = MPI::COMM_WORLD.Get_rank();
	numProcesses = MPI::COMM_WORLD.Get_size();
		
	// instantiate the random number generator for this object
	ranPtr = new MbRandom;
	
	// remember the locations of important objects
	franchisePtr = fp;
	modelPtr     = mp;
	settingsPtr  = sp;
	
	// initialize some important parameters
	menuAlpha             = settingsPtr->getMenuAlpha();
	taxonNames            = tn;
	numAuxiliaryMenuItems = settingsPtr->getNumAuxiliaryMenuItems();
	parmType              = franchisePtr->getParmType();
	parmTreatment         = franchisePtr->getParmTreatment();
	
	// initialize a vector holding the potential menu item indices
	for (int i=0; i<franchisePtr->getNumPatronsInFranchise(); i++)
		availableMenuItemIndices.insert( i );

	// initialize the franchise headquarters
	initializeFranchiseHq(mi, ti, si);

	// open file for output
	openFile();
	
	//print();
}

FranchiseHq::~FranchiseHq(void) {

	for (std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin(); m != menuItems.end(); m++)
		delete (*m);
	delete ranPtr;
	closeFile();
}

bool FranchiseHq::acceptOrReject(double lnX) {

	if ( lnX < -300.0 )
		return false;
	else if ( lnX > 0.0 )
		return true;
	else if ( ranPtr->uniformRv() < exp(lnX) )
		return true;
	return false;
}

MenuItem* FranchiseHq::addMenuItem(void) {

	MenuItem* m = new MenuItem(ranPtr, modelPtr, settingsPtr, parmType, taxonNames);
	m->setIndex( getNextMenuItemIndex() );
	menuItems.insert( m );
	return m;
}

MenuItem* FranchiseHq::addMenuItem(long int initSeed) {

	MenuItem* m = new MenuItem(ranPtr, modelPtr, settingsPtr, parmType, taxonNames, initSeed);
	m->setIndex( getNextMenuItemIndex() );
	menuItems.insert( m );
	return m;
}

void FranchiseHq::addTableImage(int rIdx, int tIdx, MenuItem* m) {

	TableImageFactory& tif = TableImageFactory::tableImageFactoryInstance(); // get a reference to the table image factory
	TableImage* ti = tif.getTableImage();                                    // get a fresh table image from the factory
	ti->setTableIndex(tIdx);                                                 // set the table index (the index of the table this is an image of)
	ti->setRestaurantIndex(rIdx);                                            // set the restaurant index (which restaurant is the orginal table in)
	m->addTableImage(ti);                                                    // have the menu item add the table image to itself (also sets the menu item index for the table image)
}

void FranchiseHq::checkAuxiliaryMenuItems(void) {

	// count how many menu items are unoccuppied
	int numUnoccuppiedMenuItems = getNumUnoccuppiedMenuItems();
		
	// add menu items, if necessary
	if ( numUnoccuppiedMenuItems < numAuxiliaryMenuItems )
		{
		for (int i=0; i<numAuxiliaryMenuItems-numUnoccuppiedMenuItems; i++)
			MenuItem* m = addMenuItem( ranPtr->getSeed() );
		}
}

void FranchiseHq::checkConsistency(void) {

	for (int i=0; i<numProcesses; i++)
		{
		if (processId == i)
			{
			print();
			}
		getchar();
		MPI::COMM_WORLD.Barrier();
		}
}

void FranchiseHq::clearTableImages(void) {

	for (std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin(); m != menuItems.end(); m++)
		(*m)->clearTableImages();
}

void FranchiseHq::closeFile(void) {

	if ( (parmTreatment == PARM_RV || parmTreatment == PARM_SAME) && processId == 0 )
		hqStrm.close();
}

MenuItem* FranchiseHq::findMenuItemIndexed(int idx) {

	for (std::set<MenuItem*,comp_menuItem>::iterator it=menuItems.begin(); it != menuItems.end(); it++)
		{
		if ( (*it)->getIndex() == idx )
			return (*it);
		}
	return NULL;
}

MenuItem* FranchiseHq::findMenuItemWithTable(int rIdx, int tIdx) {

	for (std::set<MenuItem*,comp_menuItem>::iterator it=menuItems.begin(); it != menuItems.end(); it++)
		{
		if ( (*it)->findTableImage(rIdx, tIdx) != NULL )
			return (*it);
		}
	return NULL;
}

int FranchiseHq::getIndexOfMenuItemWithTable(int rIdx, int tIdx) {

	for (std::set<MenuItem*,comp_menuItem>::iterator it=menuItems.begin(); it != menuItems.end(); it++)
		{
		if ( (*it)->findTableImage(rIdx, tIdx) != NULL )
			return (*it)->getIndex();
		}
	return -1;
}

int FranchiseHq::getNextMenuItemIndex(void) {

	std::set<int>::iterator it=availableMenuItemIndices.begin();
	int idx = (*it);
	availableMenuItemIndices.erase(idx);
	return idx;
}

int FranchiseHq::getNumOccuppiedMenuItems(void) {

	int n = 0;
	for (std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin(); m != menuItems.end(); m++)
		{
		if ( (*m)->getNumTableImages() > 0 )
			n++;
		}
	return n;
}

int FranchiseHq::getNumUnoccuppiedMenuItems(void) {

	int n = 0;
	for (std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin(); m != menuItems.end(); m++)
		{
		if ( (*m)->getNumTableImages() == 0 )
			n++;
		}
	return n;
}

void FranchiseHq::getMenuItemInfo(std::map<int,int>& mii) {

	mii.clear();
	for (std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin(); m != menuItems.end(); m++)
		mii.insert( std::make_pair((*m)->getIndex(), (*m)->getInitialParameterSeed()) );
}

int* FranchiseHq::getMenuItemInfo(int& n) {

	n = menuItems.size() * 2;
	int* x = new int[n];
	int i = 0;
	for (std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin(); m != menuItems.end(); m++)
		{
		x[i++] = (*m)->getIndex();
		x[i++] = (*m)->getInitialParameterSeed();
		}
	return x;
}

double* FranchiseHq::getStickWeights(int& n) {

	n = menuItems.size();
	double* x = new double[n];
	int i = 0;
	for (std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin(); m != menuItems.end(); m++)
		{
		x[i++] = (*m)->getStickLength();
		}
	return x;
}

int* FranchiseHq::getTableImageInfo(int& n) {

	n = 0;
	for (std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin(); m != menuItems.end(); m++)
		{
		std::set<TableImage*>& menuTableImages = (*m)->getTableImages();
		n += menuTableImages.size();
		}
	n *= 3;
	int* x = new int[n];
	
	int i = 0;
	for (std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin(); m != menuItems.end(); m++)
		{
		int mIdx = (*m)->getIndex();
		std::set<TableImage*>& menuTableImages = (*m)->getTableImages();
		for (std::set<TableImage*>::iterator t=menuTableImages.begin(); t != menuTableImages.end(); t++)	
			{
			x[i++] = (*t)->getRestaurantIndex();
			x[i++] = (*t)->getTableIndex();
			x[i++] = mIdx;
			}
		}
	return x;
}

int FranchiseHq::getNumTablesAtMenuItems(void) {

	int n = 0;
	for (std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin(); m != menuItems.end(); m++)
		n += (*m)->getNumTableImages();
	return n;
}

void FranchiseHq::getStringWithAllParameters(std::string& s) {

	for (std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin(); m != menuItems.end(); m++)
		{
		if ( (*m)->getNumTableImages() > 0 )
			{
			std::string menuParameterStr = (*m)->getParameterString();
			char temp[20];
			sprintf(temp, "%d", (*m)->getIndex());
			s += "(";
			s += temp;
			s += ":";
			s += '\t';
			s += menuParameterStr;
			s += ")";
			s += '\t';
			}
		}
}

void FranchiseHq::getTableMenuAssociation(std::map<TableImage*,MenuItem*>& kv) {

	for (std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin(); m != menuItems.end(); m++)
		{
		std::set<TableImage*>& menuTableImages = (*m)->getTableImages();
		for (std::set<TableImage*>::iterator t=menuTableImages.begin(); t != menuTableImages.end(); t++)
			kv.insert(std::make_pair( (*t), (*m) ));
		}
}

void FranchiseHq::initializeFranchiseHq(std::vector<TableImage*>& tableImages) {

	// count the number of restaurants
	std::set<int> restIds;
	for (int i=0; i<tableImages.size(); i++)
		{
		TableImage* t = tableImages[i];
		restIds.insert( t->getRestaurantIndex() );
		}
	numRestaurants = restIds.size();
		
	if (parmTreatment == PARM_RV)
		{
		// initialize franchise headquarters using DPP
		
		// i. initialze the menu items that have restaurant tables assigned
		for (int i=0; i<tableImages.size(); i++)
			{
			//std::cout << "receiveBuf[" << i << "] = " << receiveBuf[i] << std::endl;
			TableImage* t = tableImages[i];
			double probNew = menuAlpha / (i + menuAlpha);
			if (ranPtr->uniformRv() < probNew)
				{
				// assign table to a new menu item
				MenuItem* m = addMenuItem();
				m->addTableImage(t);
				}
			else 
				{
				// assign table to a pre-existing menu item
				MenuItem* m = pickMenuItemAtRandom(ranPtr);
				if (m == NULL)
					Msg::error("Problem initializing program");
				m->addTableImage(t);
				}
			}
			
		// ii. instantiate the auxiliary menu items
		for (int i=0; i<numAuxiliaryMenuItems; i++)
			MenuItem* m = addMenuItem();
			
		// iii. determine the stick breaking weight for each menu item
		setStickLengths();
		}
	else if (parmTreatment == PARM_SAME)
		{
		// all of the restaurants share a single parameter
		MenuItem* m = addMenuItem();
		for (int i=0; i<tableImages.size(); i++)
			{
			TableImage* t = tableImages[i];
			if ( m->isTableAssignedToMenuItem( t->getRestaurantIndex(), t->getTableIndex() ) == true )
				std::cout << "already found table " << t << " on MenuItem" << std::endl;
			m->addTableImage(t);
			if (m->getParmType() == PARM_TREE)
				{
				ParmTree* treePtr = dynamic_cast<ParmTree *>(m->getParameterPtr());
				if ( treePtr == 0 )
					Msg::error("Cannot find ParmTree");
				}
			}
		}
	else 
		{
		// each restaurant has a different parameter. Note that we assume that each
		// restaurant has a single table in it.
		for (int i=0; i<tableImages.size(); i++)
			{
			TableImage* t = tableImages[i];
			MenuItem* m = addMenuItem();
			m->addTableImage(t);
			if (m->getParmType() == PARM_TREE)
				{
				ParmTree* treePtr = dynamic_cast<ParmTree *>(m->getParameterPtr());
				if ( treePtr == 0 )
					Msg::error("Cannot find ParmTree");
				}
			}
		}		
		
	// make certain that the tables in the restaurants reflect the seating of their images in the head quarters
	setTableMenuAssignments();
}

void FranchiseHq::initializeFranchiseHq(std::vector<MenuItemInfo*>& mi, std::vector<TableInfo*>& ti, std::vector<double>& si) {

	// add the menu items to the franchise hq image
	int i = 0;
	for (std::vector<MenuItemInfo*>::iterator it=mi.begin(); it != mi.end(); it++)
		{
		// set the menu items
		int idx = (*it)->getIndex();
		double stickSize = si[i++];
		long int parmSeed = (*it)->getInitialParameterSeed();
		MenuItem* m = new MenuItem(ranPtr, modelPtr, settingsPtr, parmType, taxonNames, parmSeed);
		m->setIndex(idx);
		m->setStickLength(stickSize);
		availableMenuItemIndices.erase(idx);
		menuItems.insert( m );
		}
		
	// set the tables
	std::set<int> restIds;
	TableImageFactory& tif = TableImageFactory::tableImageFactoryInstance(); 
	for (std::vector<TableInfo*>::iterator it=ti.begin(); it != ti.end(); it++)
		{
		int rIdx = (*it)->getRestaurantIndex();
		int tIdx = (*it)->getTableIndex();
		int mIdx = (*it)->getMenuItemIndex();
		restIds.insert(rIdx);
		TableImage* ti = tif.getTableImage();
		ti->setRestaurantIndex( rIdx );
		ti->setTableIndex( tIdx );
		MenuItem* m = findMenuItemIndexed( mIdx );
		if (m == NULL)
			Msg::error("Cannot find menu item in Franchise Headquarters Image");
		m->addTableImage(ti);
		}
	numRestaurants = restIds.size();

	// make certain that the tables in the restaurants reflect the seating of their images in the head quarters
	setTableMenuAssignments();
}

void FranchiseHq::openFile(void) {

	if ( (parmTreatment == PARM_RV || parmTreatment == PARM_SAME) && processId == 0 )
		{
		std::string fn = settingsPtr->getOutputFileName() + ".hq." + franchisePtr->getParmName() + ".out";
		hqStrm.open( fn.c_str(), std::ios::out );
		if (!hqStrm) 
			Msg::error("Cannot open file \"" + fn + "\"");
		}
}

MenuItem* FranchiseHq::pickMenuItemAtRandom(MbRandom* rp) {

	std::vector<double> probs;
	for (std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin(); m != menuItems.end(); m++)
		probs.push_back( (*m)->getNumTableImages() );
	double sum = 0.0;
	for (std::vector<double>::iterator p=probs.begin(); p != probs.end(); p++)
		sum += (*p);
	for (std::vector<double>::iterator p=probs.begin(); p != probs.end(); p++)
		(*p) /= sum;
	double u = rp->uniformRv();
	int i = 0;
	sum = 0.0;
	for (std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin(); m != menuItems.end(); m++)
		{
		sum += probs[i];
		if (u < sum)
			return (*m);
		i++;
		}
	return NULL;
}

MenuItem* FranchiseHq::pickMenuItemFromStickWeights(MbRandom* rp) {

	double u = rp->uniformRv();
	double sum = 0.0;
	for (std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin(); m != menuItems.end(); m++)
		{
		sum += (*m)->getStickLength();
		if (u < sum)
			return (*m);
		}
	Msg::error("Problem picking menu item from stick weights");
	return NULL;
}

void FranchiseHq::print(void) {

	std::cout << "Franchise Headquarters (" << processId <<"," << menuItems.size() << "):" << std::endl;
	for (std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin(); m != menuItems.end(); m++)
		{
		(*m)->print();
		}
}

void FranchiseHq::printInfo(void) {

	std::string temp[3] = { "Independent for each gene", "Same across genes", "Hierarchical Dirichlet Process Prior" }; 
	std::cout << "   * " << franchisePtr->getFranchiseName()                                                 << std::endl;
	std::cout << "     - Parameter treatment                 = " << temp[parmTreatment]                      << std::endl;
	std::cout << "     - Number of restaurants               = " << numRestaurants                           << std::endl;
	std::cout << "     - Total number of patrons             = " << franchisePtr->getNumPatronsInFranchise() << std::endl; 
	std::cout << "     - Number of menu items                = " << menuItems.size();
	if (parmTreatment == PARM_RV)
		std::cout << " (" << numAuxiliaryMenuItems << " of which are auxiliary)";
	std::cout << std::endl;
	std::cout << "     - Number of tables                    = " << getNumTablesAtMenuItems()                << std::endl;
}

void FranchiseHq::removeAllTableImages(void) {

	for (std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin(); m != menuItems.end(); m++)
		(*m)->removeAllTableImages();
}

void FranchiseHq::removeMenuItem(MenuItem* m) {

	int idx = m->getIndex();
	availableMenuItemIndices.insert(idx);
	menuItems.erase(m);
	delete m;
}

void FranchiseHq::resetTableImages(std::vector<TableImage*>& tableImages) {

	for (std::vector<TableImage*>::iterator t=tableImages.begin(); t != tableImages.end(); t++)
		{
		MenuItem* m = findMenuItemIndexed( (*t)->getIndexOfAssignedMenuItem() );
		if ( m == NULL )
			Msg::error("Problem in resetTableImages");
		m->addTableImage(*t);
		}
}

void FranchiseHq::sampleStates(int n) {

	if ( processId == 0 )
		{
		if ( parmTreatment == PARM_RV )
			{
			hqStrm << n << '\t';
			hqStrm << getNumOccuppiedMenuItems() << '\t';
			std::string s = "";
			getStringWithAllParameters(s);
			hqStrm << s;
			hqStrm << std::endl;
			}
		else if ( parmTreatment == PARM_SAME )
			{
			hqStrm << n << '\t';
			std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin();
			hqStrm << (*m)->getParameterString();
			hqStrm << std::endl;
			}
		}
}

void FranchiseHq::setStickLengths(void) {

	std::vector<double> dirParms;
	for (std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin(); m != menuItems.end(); m++)
		{
		if ((*m)->getNumTableImages() > 0)
			dirParms.push_back( (double)((*m)->getNumTableImages()) );
		}
	dirParms.push_back( menuAlpha );
	
	std::vector<double> stickWeights(dirParms.size());
	ranPtr->dirichletRv(dirParms, stickWeights);
	
	int numAux = getNumUnoccuppiedMenuItems();
	std::vector<double> auxStickWeights(numAux);
	double unassignedStickLength = stickWeights[stickWeights.size()-1];
	for (int i=0; i<numAux; i++)
		{
		double b = ranPtr->betaRv(1.0, menuAlpha);
		double beta = b * unassignedStickLength;
		if (i+1 != numAux)
			auxStickWeights[i] = beta;
		else 
			auxStickWeights[i] = unassignedStickLength;
		unassignedStickLength -= beta;
		}
	int i = 0, j = 0;
	for (std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin(); m != menuItems.end(); m++)
		{
		if ((*m)->getNumTableImages() > 0)
			{
			(*m)->setStickLength( stickWeights[i] );
			i++;
			}
		else 
			{
			(*m)->setStickLength( auxStickWeights[j] );
			j++;
			}
		}
}

void FranchiseHq::setTableMenuAssignments(void) {

	for (std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin(); m != menuItems.end(); m++)
		{
		int mIdx = (*m)->getIndex();
		std::set<TableImage*>& tis = (*m)->getTableImages();
		for (std::set<TableImage*>::iterator ti=tis.begin(); ti != tis.end(); ti++)
			{
			Table* t = franchisePtr->findTable( (*ti)->getRestaurantIndex(), (*ti)->getTableIndex() );
			if ( t != NULL )
				{
				t->assignToMenuItem(mIdx);
				}
			}
		}
}

void FranchiseHq::synchronizeSeeds(void) {

	SeedFactory& sf = SeedFactory::seedFactoryInstance();
	long int sd = sf.getUniversalSeed();
	ranPtr->setSeed(sd);
	//std::cerr << "seed for synchronizing franchise headquarters = " << sd << std::endl;
}

void FranchiseHq::updateParameters(void) {

	for (std::set<MenuItem*,comp_menuItem>::iterator m=menuItems.begin(); m != menuItems.end(); m++)
		{
		if ( (*m)->getNumTableImages() > 0 )
			{
			double lnPriorProbOld = (*m)->lnPriorProbability();
			double lnProposalProb = (*m)->updateParameter();
			double lnPriorProbNew = (*m)->lnPriorProbability();

			Parm* p = (*m)->getParameterPtr();
			double localLnLikelihoodRatio = 0.0;
			std::vector<Restaurant*>& rests = franchisePtr->getRestaurants();
			for (std::vector<Restaurant*>::iterator r=rests.begin(); r != rests.end(); r++)
				{
				std::set<RateMatrix*>& affectedRateMatrices = (*r)->getRateMatrixMngrPtr()->getMatricesSharingParm(p, parmType);
				for (std::set<RateMatrix*>::iterator rm=affectedRateMatrices.begin(); rm != affectedRateMatrices.end(); rm++)
					(*rm)->updateRateMatrix(p);
				std::vector<int> affectedSites;
		        (*r)->getSiteModelsPtr()->getSitesWithParm(p, parmType, affectedSites);
				(*r)->getSiteLikesPtr()->flipActiveLikeForSites(affectedSites);
				localLnLikelihoodRatio += (*r)->setLnLikelihood(affectedSites);
				}
			
			// communicate here!!!
			double lnLikelihoodRatio = 0.0;
			MPI::COMM_WORLD.Allreduce(&localLnLikelihoodRatio, &lnLikelihoodRatio, 1, MPI::DOUBLE, MPI::SUM);
			//for (std::vector<Restaurant*>::iterator r=rests.begin(); r != rests.end(); r++)
			//	(*r)->getSiteLikesPtr()->print();
			//std::cerr << processId << " -- " << localLnLikelihoodRatio << " " << lnLikelihoodRatio << " " << ranPtr->getSeed() << std::endl;
			
			double lnR = lnLikelihoodRatio + (lnPriorProbNew - lnPriorProbOld) + lnProposalProb;
			bool acceptMove = acceptOrReject(lnR);
			if ( acceptMove == true )
				{
				//std::cout << "accepted" << std::endl;
				(*m)->keepUpdate();
				}
			else 
				{
				//std::cout << "rejected" << std::endl;
				(*m)->restore();
				for (std::vector<Restaurant*>::iterator r=rests.begin(); r != rests.end(); r++)
					{
					std::set<RateMatrix*>& affectedRateMatrices = (*r)->getRateMatrixMngrPtr()->getMatricesSharingParm(p, parmType);
					for (std::set<RateMatrix*>::iterator rm=affectedRateMatrices.begin(); rm != affectedRateMatrices.end(); rm++)
						(*rm)->restoreRateMatrix(p);
					std::vector<int> affectedSites;
					(*r)->getSiteModelsPtr()->getSitesWithParm(p, parmType, affectedSites);
					(*r)->getSiteLikesPtr()->flipActiveLikeForSites(affectedSites);
					}
				}
			}
		else 
			{
			/* The menu item does not have any table images. We draw a new value for the menu item's parameter
			   from the prior probability distribution. Once we have done that, we need to update the rate matrix
			   and transition probabilities for all of the rate matrices that have that parameter. */
			double lnProposalProb = (*m)->updateParameter();
			Parm* p = (*m)->getParameterPtr();
			std::vector<Restaurant*>& rests = franchisePtr->getRestaurants();
			for (std::vector<Restaurant*>::iterator r=rests.begin(); r != rests.end(); r++)
				{
				RateMatrixMngr* mngr = (*r)->getRateMatrixMngrPtr();
				std::set<RateMatrix*>& affectedRateMatrices = mngr->getMatricesSharingParm(p, parmType);
				for (std::set<RateMatrix*>::iterator rm=affectedRateMatrices.begin(); rm != affectedRateMatrices.end(); rm++)
					(*rm)->updateRateMatrix(p);
				}
			}
		}
}

