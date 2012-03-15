#include <iomanip>
#include <iostream>
#include <mpi.h>
#include <string>
#include "Alignment.h"
#include "CondLikes.h"
#include "Franchise.h"
#include "FranchiseHq.h"
#include "MbMatrix.h"
#include "MbRandom.h"
#include "MenuItem.h"
#include "Model.h"
#include "Msg.h"
#include "Parm_statefreqs.h"
#include "Parm_tree.h"
#include "RateMatrix.h"
#include "RateMatrixMngr.h"
#include "Restaurant.h"
#include "SeedFactory.h"
#include "Settings.h"
#include "SiteLikes.h"
#include "SiteModels.h"
#include "StateSets.h"



Model::Model(Settings* sp, std::vector<Alignment*>& alns, std::vector<std::string>& taxonNames) {

	// who am I?
	processId = MPI::COMM_WORLD.Get_rank();
	
	// remember the address of important objects
	settingsPtr = sp;
	
	// instantiate the random number generator for this object
	SeedFactory& sf = SeedFactory::seedFactoryInstance();
	localRanPtr     = new MbRandom(sf.getLocalSeed());
	universalRanPtr = new MbRandom(sf.getUniversalSeed());

	// how many states in this model?
	if (processId == 1)
		numStates = 4;
	MPI::COMM_WORLD.Bcast(&numStates, 1, MPI::INT, 1);
	MPI::COMM_WORLD.Barrier();
	
	// print model information to the screen (part 1)
	if (processId == 0)
		std::cout << "   Model:" << std::endl;

	// initialize the model
	initializeModel(alns, taxonNames);
	
	// print model information to the screen (part 2)
	if (processId == 0)
		std::cout << std::endl;
}

Model::Model(Settings* sp, std::vector<Alignment*>& fakeAlignments) {

	// who am I?
	processId = MPI::COMM_WORLD.Get_rank();

	// remember the address of important objects
	settingsPtr = sp;
	
	// instantiate the random number generator for this object
	SeedFactory& sf = SeedFactory::seedFactoryInstance();
	localRanPtr     = new MbRandom(sf.getLocalSeed());
	universalRanPtr = new MbRandom(sf.getUniversalSeed());

	// how many states in this model?
	for (int i=0; i<settingsPtr->getNumSimulatedRestaurants(); i++)
		{
		std::string fn = settingsPtr->getOutputFileName();
		char temp[50];
		sprintf(temp, "_aln_%d.in", i+1);
		fn += temp;
		Alignment* a = new Alignment( settingsPtr->getNumSimulatedTaxa(), settingsPtr->getNumSimulatedSites(), fn );
		a->setIndex(i);
		fakeAlignments.push_back( a );
		}
	numStates = 4;
	
	// print model information to the screen (part 1)
	std::cout << "   Model:" << std::endl;

	// initialize the model
	initializeSimulationModel(fakeAlignments);
	sampleStates(0);
	
	//std::vector<std::string>& taxonNames
	// print model information to the screen (part 2)
	std::cout << std::endl;
	
	// simulate data 
	simulate();
	int k = 0;
	for (std::vector<Alignment*>::iterator a=fakeAlignments.begin(); a != fakeAlignments.end(); a++)
		{
		k++;
		std::string fn = settingsPtr->getOutputFileName();
		char temp[50];
		sprintf(temp, "_aln_%d.in", k);
		fn += temp;
		std::ofstream simStrm;
		simStrm.open( fn.c_str(), std::ios::out );
		if (!simStrm) 
			Msg::error("Cannot open file \"" + fn + "\"");
		
		for (int i=0; i<(*a)->getNumTaxa(); i++)
			{
			simStrm << ">" << (*a)->getTaxonName(i) << std::endl;
			for (int j=0; j<(*a)->getNumChar(); j++)
				{
				std::string cStr = "";
				simStrm << cStr;
				}
			simStrm << std::endl;
			}
		simStrm.close();
		}
}

Model::~Model(void) {

	delete localRanPtr;
	delete universalRanPtr;
	for (std::vector<Franchise*>::iterator f=parameters.begin(); f != parameters.end(); f++)
		delete (*f);
	for (std::vector<CondLikes*>::iterator c=condLikes.begin(); c != condLikes.end(); c++)
		delete (*c);
	for (std::vector<SiteLikes*>::iterator s=siteLikes.begin(); s != siteLikes.end(); s++)
		delete (*s);
	for (std::vector<SiteModels*>::iterator m=siteModels.begin(); m != siteModels.end(); m++)
		delete (*m);
	for (std::vector<StateSets*>::iterator s=stateSets.begin(); s != stateSets.end(); s++)
		delete (*s);
}

double Model::calculateGlobalLikelihood(void) {

	// gather the log likelihoods for the restaurants this processor owns
	double localL = 0.0;
	for (std::vector<SiteLikes*>::iterator s=siteLikes.begin(); s != siteLikes.end(); s++)
		localL += (*s)->getActiveLike();
		
	// gather the log likelihoods from all processes
	double globalL = 0.0;
	MPI::COMM_WORLD.Allreduce( &localL, &globalL, 1, MPI::DOUBLE, MPI::SUM );
	
	return globalL;
}

void Model::checkHeadQuartersConsistency(int franchiseToUpdate) {

	parameters[franchiseToUpdate]->getFranchiseHqPtr()->checkConsistency();
}

std::string Model::getFranchiseName(int franchiseNum) {

	Franchise* f = parameters[franchiseNum];
	return f->getFranchiseName();
}

Franchise* Model::getTreeFranchisePtr(void) {

	for (std::vector<Franchise*>::iterator f=parameters.begin(); f != parameters.end(); f++)
		{
		if ( (*f)->getParmType() == PARM_TREE )
			return (*f);
		}
	return NULL;
}

void Model::initializeSimulationModel(std::vector<Alignment*>& alns) {

	// make the vector of taxon names
	std::vector<std::string> taxonNames;
	for (int i=0; i<alns[0]->getNumTaxa(); i++)
		taxonNames.push_back( alns[0]->getTaxonName(i) );
	
	// instantiate the franchises, one for each parameter
	parameters.push_back( new Franchise(localRanPtr, this, settingsPtr, alns, "Parm_tree", taxonNames) );
	parameters.push_back( new Franchise(localRanPtr, this, settingsPtr, alns, "Parm_leng", taxonNames) );
	parameters.push_back( new Franchise(localRanPtr, this, settingsPtr, alns, "Parm_subr", taxonNames) );
	parameters.push_back( new Franchise(localRanPtr, this, settingsPtr, alns, "Parm_freq", taxonNames) );

	// instantiate classes that are involved in calculating the likelihoods
	for (std::vector<Alignment*>::iterator a=alns.begin(); a != alns.end(); a++)
		{
		int n = (*a)->getNumChar();
		condLikes.push_back( new CondLikes(*a) );
		siteLikes.push_back( new SiteLikes(n) );
		siteModels.push_back( new SiteModels(n) );
		rateMatrixMngrs.push_back( new RateMatrixMngr(this, 4) );
		}
	for (std::vector<Franchise*>::iterator f=parameters.begin(); f != parameters.end(); f++)
		{
		std::vector<Restaurant*>& rests = (*f)->getRestaurants();
		for (int i=0; i<rests.size(); i++)
			{
			Restaurant* r = rests[i];
			r->setCondLikesPtr( condLikes[i] );
			r->setSiteLikesPtr( siteLikes[i] );
			r->setSiteModelsPtr( siteModels[i] );
			r->setModelInfo();
			r->setRateMatrixMngrPtr( rateMatrixMngrs[i] );
			}
		}
		
	// initialize the likelihoods
	std::vector<Restaurant*>& rests = parameters[0]->getRestaurants();
	for (std::vector<Restaurant*>::iterator r=rests.begin(); r != rests.end(); r++)
		{
		for (int i=0; i<(*r)->getNumPatrons(); i++)
			{
			double lnL = (*r)->lnLikelihood(i);
			(*r)->getSiteLikesPtr()->setActiveLike(i, lnL);
			}
		}
}

void Model::initializeModel(std::vector<Alignment*>& alns, std::vector<std::string>& taxonNames) {

	// instantiate the franchises, one for each parameter
	parameters.push_back( new Franchise(localRanPtr, this, settingsPtr, alns, "Parm_tree", taxonNames) );
	parameters.push_back( new Franchise(localRanPtr, this, settingsPtr, alns, "Parm_leng", taxonNames) );
	parameters.push_back( new Franchise(localRanPtr, this, settingsPtr, alns, "Parm_subr", taxonNames) );
	parameters.push_back( new Franchise(localRanPtr, this, settingsPtr, alns, "Parm_freq", taxonNames) );
	
	// set the proposal probabilities for each franchise
	proposalWeights.push_back( 3.0 );   // tree
	proposalWeights.push_back( 1.0 );   // length
	proposalWeights.push_back( 1.0 );   // subrates
	proposalWeights.push_back( 1.0 );   // state frequencies
	double sum = 0.0;
	for (int i=0; i<proposalWeights.size(); i++)
		sum += proposalWeights[i];
	for (int i=0; i<proposalWeights.size(); i++)
		proposalProbs.push_back( proposalWeights[i]/sum );

	// instantiate classes that are involved in calculating the likelihoods
	for (std::vector<Alignment*>::iterator a=alns.begin(); a != alns.end(); a++)
		{
		int n = (*a)->getNumChar();
		condLikes.push_back( new CondLikes(*a) );
		siteLikes.push_back( new SiteLikes(n) );
		siteModels.push_back( new SiteModels(n) );
		rateMatrixMngrs.push_back( new RateMatrixMngr(this, 4) );
		}
	for (std::vector<Franchise*>::iterator f=parameters.begin(); f != parameters.end(); f++)
		{
		std::vector<Restaurant*>& rests = (*f)->getRestaurants();
		for (int i=0; i<rests.size(); i++)
			{
			Restaurant* r = rests[i];
			r->setCondLikesPtr( condLikes[i] );
			r->setSiteLikesPtr( siteLikes[i] );
			r->setSiteModelsPtr( siteModels[i] );
			r->setModelInfo();
			r->setRateMatrixMngrPtr( rateMatrixMngrs[i] );
			}
		}
		
	// initialize the likelihoods
	std::vector<Restaurant*>& rests = parameters[0]->getRestaurants();
	for (std::vector<Restaurant*>::iterator r=rests.begin(); r != rests.end(); r++)
		{
		for (int i=0; i<(*r)->getNumPatrons(); i++)
			{
			double lnL = (*r)->lnLikelihood(i);
			(*r)->getSiteLikesPtr()->setActiveLike(i, lnL);
			}
		}
	double lnL = calculateGlobalLikelihood();
	if (processId == 0)
		std::cout << "   * Initial log likelihood                = " << std::fixed << std::setprecision(2) << lnL << std::endl;
		
	// initialize classes that are involved in calculating parsimony scores (the parsimony
	// score is only used to propose new trees under the biased TBR update mechanism)
	for (std::vector<Alignment*>::iterator a=alns.begin(); a != alns.end(); a++)
		stateSets.push_back( new StateSets((*a), settingsPtr) );
		
   std::set<MenuItem*,comp_menuItem> tmis = getTreeFranchisePtr()->getFranchiseHqPtr()->getMenuItems();
	for (std::set<MenuItem*,comp_menuItem>::iterator m=tmis.begin(); m != tmis.end(); m++)
		{
		ParmTree* treePtr = dynamic_cast<ParmTree *>((*m)->getParameterPtr());
		std::set<TableImage*> tis = (*m)->getTableImages();
		for (std::set<TableImage*>::iterator t=tis.begin(); t != tis.end(); t++)
			{
			for (std::vector<StateSets*>::iterator ss=stateSets.begin(); ss != stateSets.end(); ss++)
				{
				if ( (*ss)->getIndex() == (*t)->getRestaurantIndex() )
					{
					treePtr->getTree(0)->addStateSetToTree(*ss);
					treePtr->getTree(1)->addStateSetToTree(*ss);
					int n = treePtr->getTree(0)->getNumAffectedNucleotideSites();
					n += (*ss)->getNumChar();
					treePtr->getTree(0)->setNumAffectedNucleotideSites(n);
					treePtr->getTree(1)->setNumAffectedNucleotideSites(n);
					}
				}
			}
		int localN = treePtr->getTree(0)->getNumAffectedNucleotideSites();
		int globalN = 0;
		MPI::COMM_WORLD.Allreduce(&localN, &globalN, 1, MPI::INT, MPI::SUM);
		treePtr->getTree(0)->setNumAffectedNucleotideSites(globalN);
		treePtr->getTree(1)->setNumAffectedNucleotideSites(globalN);
		}
}

int Model::pickFranchise(void) {

	double u = universalRanPtr->uniformRv();
	double sum = 0.0;
	for (int i=0; i<proposalProbs.size(); i++)	
		{
		sum += proposalProbs[i];
		if (u < sum)
			return i;
		}
	return -1;
}

void Model::sampleStates(int n) {

	for (std::vector<Franchise*>::iterator f=parameters.begin(); f != parameters.end(); f++)
		(*f)->sampleStates(n);
}

void Model::simulate(void) {

	std::cout << "   Simulating data..." << std::endl;
}

void Model::updateFranchise(int franchiseToUpdate) {

	// get pointer to franchise
	Franchise* f = parameters[franchiseToUpdate];
	
	// update the franchise
	if ( f->getParmTreatment() == PARM_RV )
		{
		f->updateSeating();
		MPI::COMM_WORLD.Barrier();
		f->updateStickWeights();
		MPI::COMM_WORLD.Barrier();
		}
	f->updateParameters();
	MPI::COMM_WORLD.Barrier();
}



