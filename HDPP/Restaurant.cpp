#include <iomanip>
#include <iostream>
#include <map>
#include <mpi.h>
#include "Alignment.h"
#include "CondLikes.h"
#include "Franchise.h"
#include "FranchiseHq.h"
#include "MbRandom.h"
#include "MenuItem.h"
#include "Msg.h"
#include "Parm.h"
#include "Parm_length.h"
#include "Parm_subrates.h"
#include "Parm_statefreqs.h"
#include "Parm_tree.h"
#include "RateMatrix.h"
#include "RateMatrixMngr.h"
#include "Restaurant.h"
#include "Settings.h"
#include "SiteLikes.h"
#include "SiteModels.h"
#include "StateSets.h"
#include "Table.h"
#include "TableFactory.h"



Restaurant::Restaurant(MbRandom* rp, Settings* sp, Alignment* aln, Franchise* fp, double eK, int pt) {

	// who am I?
	processId = MPI::COMM_WORLD.Get_rank();

	// remember the address of important objects
	alignmentPtr = aln;
	ranPtr       = rp;
	settingsPtr  = sp;
	franchisePtr = fp;
	
	// set some paramters of the restaurant
	numPatrons    = alignmentPtr->getNumChar();
	alpha         = setByExpectedNumTables(eK, numPatrons);
	parmTreatment = pt;
	index         = aln->getIndex();
	numAuxiliary  = 3;
	parmType      = franchisePtr->getParmType();
	
	// initialize the table configuration
	initializeRestaurant();
	
	// open and then close file for output
	openFile();
	
	//print();
}

Restaurant::~Restaurant(void) {

	TableFactory& tf = TableFactory::tableFactoryInstance(); 
	for (std::set<Table *>::iterator t=tables.begin(); t != tables.end(); t++)
		tf.returnTableToPool(*t);
}

Table* Restaurant::addTable(void) {

	TableFactory& tf = TableFactory::tableFactoryInstance(); 
	Table* t = tf.getTable();
	t->setTableIndex( getNextTableIndex() );
	t->setRestaurantIndex( index );
	tables.insert( t );
	return t;
}

void Restaurant::addTable(Table* t) {

	t->setTableIndex( getNextTableIndex() );
	t->setRestaurantIndex( index );
	tables.insert( t );
}

double Restaurant::expNumTables(double a, int n) {

	double expectedNum = 0.0;
	for (int i=1; i<=n; i++)
		expectedNum += ( 1.0 / (i - 1.0 + a) );
	expectedNum *= a;
	return expectedNum;
}

void Restaurant::getMenuItemString(std::string& s) {

	FranchiseHq* fhq = franchisePtr->getFranchiseHqPtr();
	std::vector<int> patronMenuItems(numPatrons);
	for (std::set<Table*>::iterator t=tables.begin(); t != tables.end(); t++)
		{
		int mIdx = fhq->getIndexOfMenuItemWithTable( index, (*t)->getTableIndex() );
		std::set<int>& patronList = (*t)->getPatronList();
		for (std::set<int>::iterator p=patronList.begin(); p != patronList.end(); p++)
			patronMenuItems[(*p)] = mIdx;
		}
	for (int i=0; i<numPatrons; i++)
		{
		char temp[100];
		sprintf(temp, "%d\t", patronMenuItems[i]);
		s += temp;
		}
}

void Restaurant::getRgfString(std::string& s) {

}

Table* Restaurant::getTableIndexed(int tIdx) {

	for (std::set<Table*>::iterator t=tables.begin(); t != tables.end(); t++)
		{
		if ( (*t)->getTableIndex() == tIdx)
			return *t;
		}
	return NULL;
}

Table* Restaurant::getTableWithPatron(int patronId) {

	for (std::set<Table *>::iterator t=tables.begin(); t != tables.end(); t++)
		{
		if ( (*t)->isPatronAtTable(patronId) == true )
			return (*t);
		}
	return NULL;
}

int Restaurant::getNextTableIndex(void) {

	std::set<int>::iterator it=availableTableIndices.begin();
	int idx = (*it);
	availableTableIndices.erase(idx);
	return idx;
}

void Restaurant::initializeRestaurant(void) {

	// initialize available table indices
	for (int i=0; i<numPatrons; i++)
		availableTableIndices.insert( i );
	
	if (parmTreatment == PARM_RV)
		{
		// DPP model with seatings from prior
		for (int i=0; i<numPatrons; i++)
			{
			double probNewTable = alpha / (i + alpha);
			if (ranPtr->uniformRv() < probNewTable)
				{
				Table* t = addTable();
				t->addPatron(i);
				}
			else 
				{
				Table* t = pickTableAtRandom();
				if (t == NULL)
					Msg::error("Problem initializing program");
				t->addPatron(i);
				}
			}
		}
	else 
		{
		// we have one table with all of the patrons seated at it
		Table* t = addTable();
		for (int i=0; i<numPatrons; i++)
			t->addPatron(i);
		}
}

bool Restaurant::isTableInRestaurant(Table* t) {

	std::set<Table*>::iterator it = tables.find(t);
	if ( it != tables.end() )
		return true;
	return false;
}

double Restaurant::lnLikelihood(int& siteIdx) {

	// fill in the parameter information for the site
	ParmKey k;
	siteModelsPtr->setParmKey(siteIdx, k);

	// find the rate matrix for the site
	RateMatrix* rm = rateMatrixMngrPtr->retrieveRateMatrix( k );
	if ( rm == NULL )
		{
		rm = rateMatrixMngrPtr->getNewRateMatrix(k);
		rateMatrixMngrPtr->addMatrixToMngr(k, rm);
		}
	
	// calculate the likelihood for this site
	double lnL = lnLikelihood(siteIdx, rm);
	
	return lnL;
}

double Restaurant::setLnLikelihood(std::vector<int>& siteIndices) {

	double lnDiff = 0.0;
	for (std::vector<int>::iterator p=siteIndices.begin(); p != siteIndices.end(); p++)
		{
		// get the site index
		int siteIdx = (*p);
		
		// fill in the parameter information for the site
		ParmKey k;
		siteModelsPtr->setParmKey(siteIdx, k);

		// find the rate matrix for the site
		RateMatrix* rm = rateMatrixMngrPtr->retrieveRateMatrix( k );
		if ( rm == NULL )
			{
			rm = rateMatrixMngrPtr->getNewRateMatrix(k);
			rateMatrixMngrPtr->addMatrixToMngr(k, rm);
			}
		
		// calculate the likelihood for this site
		double lnL = lnLikelihood(siteIdx, rm);
		
		// set the active site likelihood
		siteLikesPtr->setActiveLike(siteIdx, lnL);
		
		lnDiff += siteLikesPtr->getLnDiff(siteIdx);
		}
	return lnDiff;
}

double Restaurant::lnLikelihood(int& siteIdx, MenuItem* m) {

	// fill in the parameter information for the site
	ParmKey k;
	siteModelsPtr->setParmKey(siteIdx, k);
	
	// modify the parameter information using the menu item information
	k.replaceParameterUsing(m->getParameterPtr());
	
	// find the rate matrix for the site
	RateMatrix* rm = rateMatrixMngrPtr->retrieveRateMatrix( k );
	if ( rm == NULL )
		{
		rm = rateMatrixMngrPtr->getNewRateMatrix(k);
		rateMatrixMngrPtr->addMatrixToMngr(k, rm);
		}
	
	// calculate the likelihood for this site
	double lnL = lnLikelihood(siteIdx, rm);
	
	return lnL;
}

double Restaurant::lnLikelihood(int& siteIdx, RateMatrix* rm) {

	// sum product algorithm
	Tree* t = rm->getTreePtr()->getActiveTree();
	for (int n=0; n<t->getNumNodes(); n++)
		{
		Node* p = t->getDownPassNode(n);
		if ( p->getIsLeaf() == false )
			{
			if ( p->getAnc() == t->getRoot() )
				{
				// 3-way split
				int lftIdx = p->getLft()->getIndex();
				int rhtIdx = p->getRht()->getIndex();
				int ancIdx = p->getAnc()->getIndex();
				int pIdx   = p->getIndex();
				double** tiL = rm->getTiMatrix(lftIdx).expose();
				double** tiR = rm->getTiMatrix(rhtIdx).expose();
				double** tiA = rm->getTiMatrix(p->getIndex()).expose();
				double* clL = condLikesPtr->getClsPtr(lftIdx, siteIdx);
				double* clR = condLikesPtr->getClsPtr(rhtIdx, siteIdx);
				double* clA = condLikesPtr->getClsPtr(ancIdx, siteIdx);
				double* clP = condLikesPtr->getClsPtr(pIdx,   siteIdx);
				for (int i=0; i<rm->getNumStates(); i++)
					{
					double sumL = 0.0, sumR = 0.0, sumA = 0.0;
					for (int j=0; j<rm->getNumStates(); j++)
						{
						sumL += tiL[i][j] * clL[j];
						sumR += tiR[i][j] * clR[j];
						sumA += tiA[i][j] * clA[j];
						}
					clP[i] = sumL * sumR * sumA;
					}
				}
			else 
				{
				// 2-way split
				int lftIdx = p->getLft()->getIndex();
				int rhtIdx = p->getRht()->getIndex();
				int pIdx   = p->getIndex();
				double** tiL = rm->getTiMatrix(lftIdx).expose();
				double** tiR = rm->getTiMatrix(rhtIdx).expose();
				double* clL = condLikesPtr->getClsPtr(lftIdx, siteIdx);
				double* clR = condLikesPtr->getClsPtr(rhtIdx, siteIdx);
				double* clP = condLikesPtr->getClsPtr(pIdx,   siteIdx);
				for (int i=0; i<rm->getNumStates(); i++)
					{
					double sumL = 0.0, sumR = 0.0;
					for (int j=0; j<rm->getNumStates(); j++)
						{
						sumL += tiL[i][j] * clL[j];
						sumR += tiR[i][j] * clR[j];
						}
					clP[i] = sumL * sumR;
					}
				}
			}
		}
		
	// average
	Node* r = t->getRoot()->getLft();
	std::vector<double>& f = rm->getStateFreqsPtr()->getActiveStateFreqs()->getVal();
	double* clP = condLikesPtr->getClsPtr(r->getIndex(), siteIdx);
	double like = 0.0;
	for (int i=0; i<rm->getNumStates(); i++)
		like += clP[i] * f[i];
	return log(like);
}

void Restaurant::openFile(void) {

	if ( parmTreatment == PARM_RV || parmTreatment == PARM_DIFF )
		{
		outputFileName = settingsPtr->getOutputFileName() + "." + alignmentPtr->getFileName() + "." + franchisePtr->getParmName() + ".out";
		std::ofstream restStrm( outputFileName.c_str(), std::ios::out );
		if ( !restStrm.good() ) 
			Msg::error("Cannot open restaurant file \"" + outputFileName + "\"");
		restStrm.close();
		}
}

Table* Restaurant::pickTableAtRandom(void) {

	std::vector<double> probs;
	for (std::set<Table*>::iterator t=tables.begin(); t != tables.end(); t++)
		probs.push_back( (*t)->getNumPatrons() );
	double sum = 0.0;
	for (std::vector<double>::iterator p=probs.begin(); p != probs.end(); p++)
		sum += (*p);
	for (std::vector<double>::iterator p=probs.begin(); p != probs.end(); p++)
		(*p) /= sum;
	double u = ranPtr->uniformRv();
	int i = 0;
	sum = 0.0;
	for (std::set<Table*>::iterator t=tables.begin(); t != tables.end(); t++)
		{
		sum += probs[i];
		if (u < sum)
			return (*t);
		i++;
		}
	return NULL;
}

Table* Restaurant::pickTableAtRandom(std::map<Table*,double>& lnProbs) {

	// normalize probabilities
	double lnC = (lnProbs.begin())->second;
	for (std::map<Table*,double>::iterator it=lnProbs.begin(); it != lnProbs.end(); it++)
		{
		if (it->second > lnC)
			lnC = it->second;
		}
	double sum = 0.0;
	for (std::map<Table*,double>::iterator it=lnProbs.begin(); it != lnProbs.end(); it++)
		{
		it->second -= lnC;
		if (it->second < -300.0)
			it->second = 0.0;
		else 
			it->second = exp(it->second);
		sum += it->second;
		}
	double sumInverse = 1.0 / sum;
	for (std::map<Table*,double>::iterator it=lnProbs.begin(); it != lnProbs.end(); it++)
		it->second *= sumInverse;

	// pick a table
	double u = ranPtr->uniformRv();
	sum = 0.0;
	for (std::map<Table*,double>::iterator it=lnProbs.begin(); it != lnProbs.end(); it++)
		{
		sum += it->second;
		if (u < sum)	
			return it->first;
		}
	return NULL;
}

void Restaurant::print(void) {

	std::cout << "Restaurant for alignment \"" << alignmentPtr->getFileName() << "\":" << std::endl;
	std::cout << "   Number of tables = " << tables.size() << std::endl;
	for (std::set<Table*>::iterator t=tables.begin(); t != tables.end(); t++)
		(*t)->print();
}

void Restaurant::removeTable(Table* t) {

	TableFactory& tf = TableFactory::tableFactoryInstance(); 
	int idx = t->getTableIndex();
	availableTableIndices.insert(idx);
	tf.returnTableToPool(t);
	tables.erase(t);
}

void Restaurant::sampleStates(int n) {

	if ( parmTreatment == PARM_RV )
		{
		std::ofstream restStrm( outputFileName.c_str(), std::ios::app );
		restStrm << n << '\t';
		restStrm << tables.size() << '\t';
		restStrm << "--" << '\t';
		std::string pStr = "";
		getMenuItemString(pStr);
		restStrm << pStr;
		restStrm << std::endl;
		restStrm.close();
		}
	else if ( parmTreatment == PARM_DIFF )
		{
		std::ofstream restStrm( outputFileName.c_str(), std::ios::app );
		restStrm << n << '\t';
		std::set<Table*>::iterator t=tables.begin();
		MenuItem* m = franchisePtr->getFranchiseHqPtr()->findMenuItemWithTable( index, (*t)->getTableIndex() );
		restStrm << m->getParameterString();
		restStrm << std::endl;
		restStrm.close();
		}
}

double Restaurant::setByExpectedNumTables(double target, int n) {

	double a = 0.000001;
	double ea = expNumTables(a, n);
	bool goUp;
	if (ea < target)
		goUp = true;
	else
		goUp = false;
	double increment = 0.1;
	while ( fabs(ea - target) > 0.000001 )
		{
		if (ea < target && goUp == true)
			{
			a += increment;
			}
		else if (ea > target && goUp == false)
			{
			a -= increment;
			}
		else if (ea < target && goUp == false)
			{
			increment /= 2.0;
			goUp = true;
			a += increment;
			}
		else
			{
			increment /= 2.0;
			goUp = false;
			a -= increment;
			}
		ea = expNumTables(a, n);
		}
	return a;
}

void Restaurant::setModelInfo(void) {

	if ( parmType == PARM_LENG )
		{
		for (std::set<Table*>::iterator t=tables.begin(); t != tables.end(); t++)
			{
			MenuItem* m = franchisePtr->getFranchiseHqPtr()->findMenuItemWithTable( index, (*t)->getTableIndex() );
			Parm* genericParmPtr = m->getParameterPtr();
			ParmLength* lenPtr = dynamic_cast<ParmLength *>(genericParmPtr);
			if ( lenPtr == 0 )
				Msg::error("Cannot find ParmLength");
			std::set<int>& patronList = (*t)->getPatronList();
			for (std::set<int>::iterator p=patronList.begin(); p != patronList.end(); p++)
				siteModelsPtr->setLength( (*p), lenPtr );
			}
		}
	else if ( parmType == PARM_SUBR )
		{
		for (std::set<Table*>::iterator t=tables.begin(); t != tables.end(); t++)
			{
			MenuItem* m = franchisePtr->getFranchiseHqPtr()->findMenuItemWithTable( index, (*t)->getTableIndex() );
			Parm* genericParmPtr = m->getParameterPtr();
			ParmSubRates* srPtr = dynamic_cast<ParmSubRates *>(genericParmPtr);
			if ( srPtr == 0 )
				Msg::error("Cannot find ParmSubRates");
			std::set<int>& patronList = (*t)->getPatronList();
			for (std::set<int>::iterator p=patronList.begin(); p != patronList.end(); p++)
				siteModelsPtr->setSubRates( (*p), srPtr );
			}
		}
	else if ( parmType == PARM_FREQ )
		{
		for (std::set<Table*>::iterator t=tables.begin(); t != tables.end(); t++)
			{
			MenuItem* m = franchisePtr->getFranchiseHqPtr()->findMenuItemWithTable( index, (*t)->getTableIndex() );
			Parm* genericParmPtr = m->getParameterPtr();
			ParmStateFreqs* stateFreqsPtr = dynamic_cast<ParmStateFreqs *>(genericParmPtr);
			if ( stateFreqsPtr == 0 )
				Msg::error("Cannot find ParmStateFreqs");
			std::set<int>& patronList = (*t)->getPatronList();
			for (std::set<int>::iterator p=patronList.begin(); p != patronList.end(); p++)
				siteModelsPtr->setStateFreqs( (*p), stateFreqsPtr );
			}
		}
	else if ( parmType == PARM_TREE )
		{
		for (std::set<Table*>::iterator t=tables.begin(); t != tables.end(); t++)
			{
			MenuItem* m = franchisePtr->getFranchiseHqPtr()->findMenuItemWithTable( index, (*t)->getTableIndex() );
			Parm* genericParmPtr = m->getParameterPtr();
			ParmTree* treePtr = dynamic_cast<ParmTree *>(genericParmPtr);
			if ( treePtr == 0 )
				Msg::error("Cannot find ParmTree");
			std::set<int>& patronList = (*t)->getPatronList();
			for (std::set<int>::iterator p=patronList.begin(); p != patronList.end(); p++)
				siteModelsPtr->setTree( (*p), treePtr );
			}
		}
}

#undef DEBUG_SEATING
#ifdef DEBUG_SEATING
	int whichProcessId = 0;
#endif
void Restaurant::updateSeatingForPatrons(FranchiseHq* hq) {

#	ifdef DEBUG_SEATING
	if (processId == whichProcessId)
		print();
#	endif

	// get a reference to the table factory
	TableFactory& tf = TableFactory::tableFactoryInstance(); 
	
	for (int pIdx=0; pIdx<getNumPatrons(); pIdx++)
		{
		// remove the patron from its current table
		Table* t = getTableWithPatron(pIdx);                                    // find the table with the patron
		t->removePatron(pIdx);                                                  // remove the patron from the table
		if ( t->getNumPatrons() == 0 )
			{
			MenuItem* m = hq->findMenuItemWithTable(index, t->getTableIndex()); // find the menu item with this table's image
			m->removeTableImage(index, t->getTableIndex());                     // remove the table's image from the menu item
			removeTable(t);                                                     // remove the table from the restaurant
			}
			
		// calculate the probabilities of reseating the patron at the possible tables
		std::multimap<MenuItem*,Table*> menuItemTableMap;
		std::map<Table*,double> lnProbs;
		std::set<MenuItem*> uniqueMenuItems;
		for (std::set<Table*>::iterator t=tables.begin(); t != tables.end(); t++)
			{
			MenuItem* m = hq->findMenuItemWithTable(index, (*t)->getTableIndex());
			menuItemTableMap.insert( std::make_pair(m,*t) );
			uniqueMenuItems.insert(m);
			lnProbs.insert( std::make_pair(*t,log((*t)->getNumPatrons())) );
			}
		std::set<Table*> auxiliaryTables;
		for (int i=0; i<numAuxiliary; i++)
			{
			Table* t = tf.getTable();
			MenuItem* m = hq->pickMenuItemFromStickWeights(ranPtr);
			t->assignToMenuItem(m->getIndex());
			menuItemTableMap.insert( std::make_pair(m,t) );
			auxiliaryTables.insert(t);
			uniqueMenuItems.insert(m);
			lnProbs.insert( std::make_pair(t,log(alpha/numAuxiliary)) );
			}
			
		// calculate likelihoods
		for (std::set<MenuItem*>::iterator m=uniqueMenuItems.begin(); m != uniqueMenuItems.end(); m++)
			{
			double lnX = lnLikelihood(pIdx, *m);
			for (std::multimap<MenuItem*,Table*>::iterator it=menuItemTableMap.begin(); it != menuItemTableMap.end(); it++)
				{
				if ( it->first == (*m) )
					{
					Table* t = it->second;
					std::map<Table*,double>::iterator it2 = lnProbs.find(t);
					if ( it2 == lnProbs.end() )
						Msg::error("Problem finding table");
					it2->second += lnX;
					t->setScratchVal(lnX);
					}
				}
			}
						
		// pick a table and add the patron to it
		Table* newT = pickTableAtRandom(lnProbs);
		newT->addPatron(pIdx);
		MenuItem* newM = NULL;
		for (std::multimap<MenuItem*,Table*>::iterator it=menuItemTableMap.begin(); it != menuItemTableMap.end(); it++)
			{
			if ( it->second == newT )
				{
				newM = it->first;
				break;
				}
			}
		newT->assignToMenuItem( newM->getIndex() );
		
		// reconfigure the restaurant and the headquarters
		std::set<Table*>::iterator tblit = auxiliaryTables.find(newT);
		if ( tblit != auxiliaryTables.end() )
			{
			// the chosen table is an auxiliary one and we need to add it to the restaurant and its image to the headquarters
			addTable(newT);
			hq->addTableImage( newT->getRestaurantIndex(), newT->getTableIndex(), newM );
			}
			
		// set the site likelihood appropriately
		siteLikesPtr->setActiveLike( pIdx, newT->getScratchVal() );
		
		// adjust the site model
		siteModelsPtr->setParm( pIdx, newM->getParameterPtr(), parmType ); 
			
		// print information for debugging purposes
#		ifdef DEBUG_SEATING
		if (processId == whichProcessId)
			{
			int k=0;
			for (std::multimap<Table*,double>::iterator it=lnProbs.begin(); it != lnProbs.end(); it++)
				{
				MenuItem* m = NULL;
				for (std::multimap<MenuItem*,Table*>::iterator it2=menuItemTableMap.begin(); it2 != menuItemTableMap.end(); it2++)
					{
					if ( it2->second == it->first )
						m = it2->first;
					}
				std::cerr << index << " " << k++ << " " << pIdx << " t" << it->first << " " << it->first->getTableIndex() << " m" << m << " " << it->first->getNumPatrons() << " -- " << std::fixed << std::setprecision(10) << it->second;
				if ( it->first == newT )
					std::cerr << " <- New table for seating";
				std::cerr << std::endl;
				if ( m == NULL )
					Msg::error("Cannot find menu item");
				}
			std::cerr << std::endl;
			}
#		endif

		// remove unused auxiliary tables
		for (std::set<Table*>::iterator it=auxiliaryTables.begin(); it != auxiliaryTables.end(); it++)
			{
			if ( (*it) != newT )
				tf.returnTableToPool(*it);
			}
		}
#	ifdef DEBUG_SEATING
	if (processId == whichProcessId)
		print();
#	endif
}

