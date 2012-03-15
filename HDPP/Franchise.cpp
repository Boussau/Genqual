#include <iomanip>
#include <iostream>
#include <map>
#include <mpi.h>
#include <string>
#include "Franchise.h"
#include "FranchiseHq.h"
#include "MbRandom.h"
#include "MenuItemInfo.h"
#include "Msg.h"
#include "Restaurant.h"
#include "Settings.h"
#include "Table.h"
#include "TableInfo.h"
#include "TableImage.h"
#include "TableImageFactory.h"



Franchise::Franchise(MbRandom* rp, Model* mp, Settings* sp, std::vector<Alignment*>& alns, std::string pt, std::vector<std::string>& tn) {

	// who am I?
	processId    = MPI::COMM_WORLD.Get_rank();
	numProcesses = MPI::COMM_WORLD.Get_size();

	// remember the address of important objects
	modelPtr    = mp;
	ranPtr      = rp;
	settingsPtr = sp;

	// initialize some important parameters
	parmType      = determineParmType(pt);
	parmTreatment = determineParmTreatment(pt);
	taxonNames    = tn;

	if (settingsPtr->getAreParametersEstimated() == true)
		{
		// initialize franchise
		initializeFranchise(alns, settingsPtr->getPriorMeanRest());
		
		// calculate information on the prior
		calculatePriorStatistics();
		}
	else 
		{
		// initialize franchise
		initializeFranchiseForSimulation(alns, settingsPtr->getPriorMeanRest());
		}
}

Franchise::~Franchise(void) {

	for (std::vector<Restaurant*>::iterator r=restaurants.begin(); r != restaurants.end(); r++)
		delete (*r);
}

void Franchise::calculatePriorStatistics(void) {

	if (parmTreatment != PARM_RV)
		return;
		
	// communicate the number of restaurants on each process to processId = 0
	int* numRestaurantsOnProcess = new int[numProcesses];
	for (int i=0; i<numProcesses; i++)
		numRestaurantsOnProcess[i] = 0;
	int nr = restaurants.size();
	MPI::COMM_WORLD.Gather(&nr, 1, MPI::INT, numRestaurantsOnProcess, 1, MPI::INT, 0);
	MPI::COMM_WORLD.Barrier();
	
	// communicate the number of patrons in each restaurant to procesId = 0
	int* numPatronsInRestaurant = new int[numRestaurants]; // NOTE: numRestaurants is initialized in initializeFranchise
	int* receiveCnts = new int[numProcesses];
	for (int i=0; i<numProcesses; i++)
		receiveCnts[i] = numRestaurantsOnProcess[i];
	int* displs = new int[numProcesses];
	displs[0] = 0;
	for (int i=1; i<numProcesses; i++)
		displs[i] = displs[i-1] + numRestaurantsOnProcess[i-1];
	int* sendBuf = new int[restaurants.size()];
	int i = 0;
	for (std::vector<Restaurant*>::iterator r=restaurants.begin(); r != restaurants.end(); r++)
		sendBuf[i++] = (*r)->getNumPatrons();
	MPI::COMM_WORLD.Gatherv(sendBuf, restaurants.size(), MPI::INT, numPatronsInRestaurant, receiveCnts, displs, MPI::INT, 0);
	MPI::COMM_WORLD.Barrier();
	
	// communicate the concentration parameter (alpha) for each restaurant
	double* restAlphas = new double[numRestaurants];
	double* sendDoubleBuf = new double[restaurants.size()];
	i = 0;
	for (std::vector<Restaurant*>::iterator r=restaurants.begin(); r != restaurants.end(); r++)
		sendDoubleBuf[i++] = (*r)->getAlpha();
	MPI::COMM_WORLD.Gatherv(sendDoubleBuf, restaurants.size(), MPI::DOUBLE, restAlphas, receiveCnts, displs, MPI::DOUBLE, 0);
	MPI::COMM_WORLD.Barrier();
	
	// calculate distribution for the number of menu items
	if (processId == 0)
		{
		int numReps = 10000;
		std::vector<int> menuItemHist;
		double menuAlpha = settingsPtr->getMenuAlpha();
		for (int rep=0; rep<numReps; rep++)
			{
			int nt = 0;
			for (int i=0; i<numRestaurants; i++)
				nt += drawNumberOfTablesInCpp( restAlphas[i], numPatronsInRestaurant[i] );
			int nm = drawNumberOfTablesInCpp( menuAlpha, nt );
			if ( nm >= menuItemHist.size() )
				menuItemHist.resize(nm+3, 0);
			menuItemHist[nm]++;
			}
			
		double sum = 0;
		for (int i=0; i<menuItemHist.size(); i++)
			sum += i * menuItemHist[i];
		double mean = sum / numReps;

		std::cout << "     - E(Number of restaurant tables)      = " << std::fixed << std::setprecision(2) << settingsPtr->getPriorMeanRest() << std::endl;
		std::cout << "     - E(Number of menu items)             = " << std::fixed << std::setprecision(2) << mean << std::endl;
		//for (int i=1; i<menuItemHist.size(); i++)
		//	std::cout << "     -                                       " << std::setw(4) << i << "  " << (double)menuItemHist[i] / numReps << std::endl;
		}
	
	// free memory
	delete [] numRestaurantsOnProcess;
	delete [] numPatronsInRestaurant;
	delete [] receiveCnts;
	delete [] displs;
	delete [] sendBuf;
	delete [] restAlphas;
	delete [] sendDoubleBuf;
	
	MPI::COMM_WORLD.Barrier();
}

int Franchise::determineParmTreatment(std::string pt) {

	if ( pt == "Parm_tree" )
		return settingsPtr->getTreatmentTopology();
	else if ( pt == "Parm_leng" )
		return settingsPtr->getTreatmentLength();
	else if ( pt == "Parm_subr" )
		return settingsPtr->getTreatmentSubRates();
	else if ( pt == "Parm_freq" )
		return settingsPtr->getTreatmentBasefreqs();
	else 
		Msg::error("Cannot determine parameter type");
	return -1;
}

int Franchise::determineParmType(std::string pt) {

	if ( pt == "Parm_tree" )
		{
		franchiseName = "Tree Franchise";
		parmName = "tau";
		return PARM_TREE;
		}
	else if ( pt == "Parm_leng" )
		{
		franchiseName = "Tree-Length Franchise";
		parmName = "length";
		return PARM_LENG;
		}
	else if ( pt == "Parm_subr" )
		{
		franchiseName = "Substitution Rate Franchise";
		parmName = "rates";
		return PARM_SUBR;
		}
	else if ( pt == "Parm_freq" )
		{
		franchiseName = "Pi(e) Franchise";
		parmName = "pi";
		return PARM_FREQ;
		}
	else 
		Msg::error("Cannot determine parameter type");
	return -1;
}

int Franchise::drawNumberOfTablesInCpp(double a, int n) {

	int nr = 0;
	for (int i=0; i<n; i++)
		{
		double probNew = a / (a + i);
		if (ranPtr->uniformRv() < probNew)
			nr++;
		}
	return nr;
}

Table* Franchise::findTable(int rIdx, int tIdx) {

	for (std::vector<Restaurant*>::iterator r=restaurants.begin(); r != restaurants.end(); r++)
		{
		if ( (*r)->getIndex() == rIdx )
			{
			Table* t = (*r)->getTableIndexed(tIdx);
			if ( t != NULL )
				return t;
			}
		}
	return NULL;
}

void Franchise::gatherRestaurantTables(std::vector<TableImage*>& tableImages) {
	
	/* First, I gather the information on the number of tables that a particular 
	   process has. Note that the number of tables (numTablesOnProc) is a sum of 
	   the number of tables assigned to each restaurant assigned to the processor. */
	int numTablesOnProc = 0;
	for (std::vector<Restaurant*>::iterator r=restaurants.begin(); r != restaurants.end(); r++)
		numTablesOnProc += (*r)->getNumTables();
	int* tableCnts = new int [numProcesses];
	for (int i=0; i<numProcesses; i++)
		tableCnts[i] = 0;
	MPI::COMM_WORLD.Gather(&numTablesOnProc, 1, MPI::INT, tableCnts, 1, MPI::INT, 0);
	MPI::COMM_WORLD.Barrier();
		
	/* Second, I gather the information to the tables from each process with ID not equal to
	   zero to the process with ID 0. */
	int *receiveBuf, *sendBuf, *receiveCnts, *displs, totalNumTables = 0;
	if (processId == 0)
		{
		for (int i=0; i<numProcesses; i++)
			totalNumTables += tableCnts[i];
		receiveBuf = new int[2*totalNumTables];
		receiveCnts = new int[numProcesses];
		for (int i=0; i<numProcesses; i++)
			receiveCnts[i] = tableCnts[i]*2;
		displs = new int[numProcesses];
		displs[0] = 0;
		for (int i=1; i<numProcesses; i++)
			displs[i] = displs[i-1] + tableCnts[i-1]*2;
		}
	MPI::COMM_WORLD.Barrier();

	sendBuf = new int[2*numTablesOnProc];
	int i = 0;
	for (std::vector<Restaurant*>::iterator r=restaurants.begin(); r != restaurants.end(); r++)
		{
		std::set<Table*>& tableSet = (*r)->getTables();
		for (std::set<Table*>::iterator t=tableSet.begin(); t != tableSet.end(); t++)
			{
			sendBuf[i++] = (*t)->getRestaurantIndex();
			sendBuf[i++] = (*t)->getTableIndex();
			}
		}

	MPI::COMM_WORLD.Gatherv(sendBuf, numTablesOnProc*2, MPI::INT, receiveBuf, receiveCnts, displs, MPI::INT, 0);
	
	/* Third, fill in the vector holding the table images. */
	if (processId == 0)
		{
		TableImageFactory& tif = TableImageFactory::tableImageFactoryInstance(); 
		for (int i=0; i<totalNumTables; i++)
			{
			int rIdx = receiveBuf[2*i+0];
			int tIdx = receiveBuf[2*i+1];
			TableImage* ti = tif.getTableImage();
			ti->setRestaurantIndex(rIdx);
			ti->setTableIndex(tIdx);
			tableImages.push_back( ti );
			}
		}

	/* Finally, free memory that was used locally. */
	delete [] tableCnts;
	delete [] sendBuf;
	if (processId == 0)
		{
		delete [] receiveBuf;
		delete [] receiveCnts;
		delete [] displs;
		}
}

int Franchise::getNumPatronsOnProcess(void) {

	int n = 0;
	for (std::vector<Restaurant*>::iterator r=restaurants.begin(); r != restaurants.end(); r++)
		n += (*r)->getNumPatrons();
	return n;
}

double Franchise::lnLikelihood(MenuItem* m) {

	return 0.0;
}

void Franchise::initializeFranchise(std::vector<Alignment*>& alns, double eK) {

	/* Initializing the franchise: I start by instantiating the restaurants. Each
	   restaurant is for one alignment, and the processes holding the alignments
	   are labelled from 0 - numProccesses-1. When a restaurant is instantiated, 
	   all of the nucleotide sites are assigned to tables.
	   
	   After the restaurant is instantiated, I can then instantiate the franchise
	   "headquarters". This involves assigning the tables from the restaurants to
	   menu items. However, to do this, I must first communicate the table information
	   to process ID 0. I do this with two Gather function calls. The first gathers
	   the information on the number of tables that each process has. The second 
	   gathers the table information to process ID 0. */
	   
	/* Instantiate the restaurants, one for each alignment, for this process. */ 
	for (std::vector<Alignment*>::iterator a=alns.begin(); a != alns.end(); a++)
		restaurants.push_back( new Restaurant(ranPtr, settingsPtr, *a, this, eK, parmTreatment) );
	MPI::COMM_WORLD.Barrier();

	/* Determine how many restaurants there are in total, across all the processes. */
	numRestaurants = 0;
	int nr = restaurants.size();
	MPI::COMM_WORLD.Allreduce(&nr, &numRestaurants, 1, MPI::INT, MPI::SUM);
	//MPI::COMM_WORLD.Bcast(&numRestaurants, 1, MPI::INT, 0); // CHECK: Is this line necessary?
	
	/* Determine how many patrons there are in total, across all restaurants. */
	numPatronsInFranchise = 0;
	int np = getNumPatronsOnProcess();
	MPI::COMM_WORLD.Allreduce(&np, &numPatronsInFranchise, 1, MPI::INT, MPI::SUM);

	/* Initialize the franchise "headquarters" on process 0. */
	std::vector<TableImage*> tableImages;
	gatherRestaurantTables(tableImages);
	if (processId == 0)
		{
		headQuarters = new FranchiseHq(modelPtr, this, settingsPtr, taxonNames, tableImages);
		headQuarters->printInfo();
		}
	MPI::COMM_WORLD.Barrier();

	/* Initialize the franchise headquarters on all other processes except 0. */
	int numHqMenuItems = 0, numHqTableImages = 0, numSticks = 0, *hqMenuItems, *hqTableImages;
	double* stickWeightVec;
	if (processId == 0)
		{
		hqMenuItems = headQuarters->getMenuItemInfo(numHqMenuItems);
		hqTableImages = headQuarters->getTableImageInfo(numHqTableImages);
		stickWeightVec = headQuarters->getStickWeights(numSticks);
		}
	MPI::COMM_WORLD.Bcast(&numHqMenuItems, 1, MPI::INT, 0);
	MPI::COMM_WORLD.Bcast(&numHqTableImages, 1, MPI::INT, 0);
	MPI::COMM_WORLD.Bcast(&numSticks, 1, MPI::INT, 0);
	if (processId != 0)
		{
		hqMenuItems = new int[numHqMenuItems];
		hqTableImages = new int[numHqTableImages];
		stickWeightVec = new double[numHqTableImages];
		}
	MPI::COMM_WORLD.Bcast(hqMenuItems, numHqMenuItems, MPI::INT, 0);
	MPI::COMM_WORLD.Bcast(hqTableImages, numHqTableImages, MPI::INT, 0);
	MPI::COMM_WORLD.Bcast(stickWeightVec, numSticks, MPI::DOUBLE, 0);
	if (processId != 0)
		{
		/* Gather the information gathered from the other processes into a convenient vector. */
		std::vector<MenuItemInfo*> menuInfo;
		for (int i=0; i<numHqMenuItems; i+=2)
			menuInfo.push_back( new MenuItemInfo(hqMenuItems[i+0], hqMenuItems[i+1]) );
		std::vector<TableInfo*> tableInfo;
		for (int i=0; i<numHqTableImages; i+=3)
			tableInfo.push_back( new TableInfo(hqTableImages[i+0], hqTableImages[i+1], hqTableImages[i+2]) );
		std::vector<double> stickInfo;
		for (int i=0; i<numSticks; i++)
			stickInfo.push_back(stickWeightVec[i]);

		/* instantiate the headquarters on processes with ids > 0 */
		headQuarters = new FranchiseHq(modelPtr, this, settingsPtr, taxonNames, menuInfo, tableInfo, stickInfo);
		//headQuarters->printInfo();
		
		/* delete the temporary information */
		for (std::vector<MenuItemInfo*>::iterator it=menuInfo.begin(); it != menuInfo.end(); it++)
			delete *it;
		for (std::vector<TableInfo*>::iterator it=tableInfo.begin(); it != tableInfo.end(); it++)
			delete *it;
		}
		
	/* Synchronize the seeds of the franchise headquarters. */
	synchronizeFranchiseHqSeeds();
	headQuarters->setStickLengths();
	
	/* The vectors containing the head quarters menu items and table images are dynamically allocated and
	   returned in the headQuarters->getMenuItemInfo and headQuarters->getTableImageInfo functions. However,
	   this function is responsible for freeing those vectors. */
	delete [] hqMenuItems;
	delete [] hqTableImages;
	MPI::COMM_WORLD.Barrier();
	
	/* print results */
#	if 1
	headQuarters->print();
#	endif

	//headQuarters->checkConsistency();
}

void Franchise::initializeFranchiseForSimulation(std::vector<Alignment*>& alns, double eK) {

	// instantiate the restaurants
	for (std::vector<Alignment*>::iterator a=alns.begin(); a != alns.end(); a++)
		restaurants.push_back( new Restaurant(ranPtr, settingsPtr, *a, this, eK, parmTreatment) );
	numRestaurants = restaurants.size();
	numPatronsInFranchise = getNumPatronsOnProcess();

	// set up all of the table images
	TableImageFactory& tif = TableImageFactory::tableImageFactoryInstance(); 
	std::vector<TableImage*> tableImages;
	for (std::vector<Restaurant*>::iterator r=restaurants.begin(); r != restaurants.end(); r++)
		{
		std::set<Table*>& tableSet = (*r)->getTables();
		for (std::set<Table*>::iterator t=tableSet.begin(); t != tableSet.end(); t++)
			{
			TableImage* ti = tif.getTableImage();
			ti->setRestaurantIndex( (*t)->getRestaurantIndex() );
			ti->setTableIndex( (*t)->getTableIndex() );
			tableImages.push_back( ti );
			}
		}

	// instantiate the head quarters
	headQuarters = new FranchiseHq(modelPtr, this, settingsPtr, taxonNames, tableImages);
	headQuarters->setStickLengths();
	headQuarters->printInfo();
}

void Franchise::sampleStates(int n) {

	for (std::vector<Restaurant*>::iterator r=restaurants.begin(); r != restaurants.end(); r++)
		(*r)->sampleStates(n);
	headQuarters->sampleStates(n);
}

void Franchise::shareRestaurantTableInformation(std::vector<TableImage*>& tableImages) {

	/* First, gather the information on the number of tables that each processor has. */ 
	int numTablesOnProc = 0;
	for (std::vector<Restaurant*>::iterator r=restaurants.begin(); r != restaurants.end(); r++)
		numTablesOnProc += (*r)->getNumTables();
	int* tableCnts = new int [numProcesses];
	for (int i=0; i<numProcesses; i++)
		tableCnts[i] = 0;
	MPI::COMM_WORLD.Allgather(&numTablesOnProc, 1, MPI::INT, tableCnts, 1, MPI::INT);

	/* Second, gather the information to the tables from each process with ID not equal to
	   zero. */
	int totalNumTables = 0;
	for (int i=0; i<numProcesses; i++)
		totalNumTables += tableCnts[i];

	int* receiveBuf = new int[3*totalNumTables];
	int* receiveCnts = new int[numProcesses];
	for (int i=0; i<numProcesses; i++)
		receiveCnts[i] = tableCnts[i]*3;
	int* displs = new int[numProcesses];
	displs[0] = 0;
	for (int i=1; i<numProcesses; i++)
		displs[i] = displs[i-1] + tableCnts[i-1]*3;
	int* sendBuf = new int[3*numTablesOnProc];
	int i = 0;
	for (std::vector<Restaurant*>::iterator r=restaurants.begin(); r != restaurants.end(); r++)
		{
		std::set<Table*>& tableSet = (*r)->getTables();
		for (std::set<Table*>::iterator t=tableSet.begin(); t != tableSet.end(); t++)
			{
			sendBuf[i++] = (*t)->getRestaurantIndex();
			sendBuf[i++] = (*t)->getTableIndex();
			sendBuf[i++] = (*t)->getIndexOfAssignedMenuItem();
			}
		}
	MPI::COMM_WORLD.Allgatherv(sendBuf, numTablesOnProc*3, MPI::INT, receiveBuf, receiveCnts, displs, MPI::INT);

	/* Third, fill in the vector holding the table images. */
	TableImageFactory& tif = TableImageFactory::tableImageFactoryInstance(); 
	for (int i=0; i<totalNumTables; i++)
		{
		int rIdx = receiveBuf[3*i+0];
		int tIdx = receiveBuf[3*i+1];
		int mIdx = receiveBuf[3*i+2];
		TableImage* ti = tif.getTableImage();
		ti->setRestaurantIndex(rIdx);
		ti->setTableIndex(tIdx);
		ti->assignToMenuItem(mIdx);
		tableImages.push_back( ti );
		}
		
	/* Finally, free memory that was used locally. */
	delete [] tableCnts;
	delete [] sendBuf;
	delete [] receiveBuf;
	delete [] receiveCnts;
	delete [] displs;

#	if 0
	if (processId == 0)
		{
		for (std::vector<TableImage*>::iterator t=tableImages.begin(); t != tableImages.end(); t++)
			(*t)->print();
		}
#	endif
}

void Franchise::synchronizeFranchises(void) {

	// remove all of the table images from the head quarters
	headQuarters->removeAllTableImages();
	MPI::COMM_WORLD.Barrier();
	
	// gather information on the current table configurations in each restaruant
	std::vector<TableImage*> tableImages;
	shareRestaurantTableInformation(tableImages);
	MPI::COMM_WORLD.Barrier();
	headQuarters->resetTableImages(tableImages);
	MPI::COMM_WORLD.Barrier();
	
	// add new menu items if the number of unoccupied menu items is less than
	// the number of auxiliary menu items
	headQuarters->checkAuxiliaryMenuItems();
	MPI::COMM_WORLD.Barrier();

}

void Franchise::synchronizeFranchiseHqSeeds(void) {

	headQuarters->synchronizeSeeds();
}

void Franchise::updateSeating(void) {

	for (std::vector<Restaurant*>::iterator r=restaurants.begin(); r != restaurants.end(); r++)
		(*r)->updateSeatingForPatrons(headQuarters);
	MPI::COMM_WORLD.Barrier();
	synchronizeFranchises();
}

void Franchise::updateStickWeights(void) {

	headQuarters->setStickLengths();
}

void Franchise::updateParameters(void) {

	headQuarters->updateParameters();
}

