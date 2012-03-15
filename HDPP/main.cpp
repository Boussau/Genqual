#include <iostream>
#include <mpi.h>
#include "Alignment.h"
#include "IoManager.h"
#include "Mcmc.h"
#include "Model.h"
#include "Msg.h"
#include "SeedFactory.h"
#include "Settings.h"
#include "QualityScores.h"


// function prototypes
bool initialize(int& argc, char *argv[]);
void initializeSeed(void);
void printFarewell(void);
void printGreeting(void);
void readAlignments(Settings &userSettings, std::vector<Alignment *> &alns, IoManager &fileMngr, std::vector<std::string>& taxonNames);
void readQualityScores(Settings &userSettings, std::vector<QualityScores *> &qss, IoManager &fileMngr);
void associate(std::vector<Alignment *> &alns, std::vector<QualityScores *> &qss);


int main(int argc, char *argv[]) {
    
	// initialize
	if ( initialize(argc, argv) == false )
		Msg::error("Problem initializing program");
	
	// print heading
	printGreeting();

	// instantiate a random number object with a different seed for each process
	initializeSeed();

	// get the user settings
	Settings userSettings(argc, argv);
	userSettings.print();

	// run the analysis
	std::vector<Alignment *> myAlignments;
    std::vector<QualityScores > myQualityScores; //To Implement
	if (userSettings.getAreParametersEstimated() == true)
		{
		// estimate parameters 
		
		// read the alignments
		IoManager fileMngr;
		fileMngr.setFilePath( userSettings.getPathName() );
		std::vector<std::string> taxonNames;
		readAlignments( userSettings, myAlignments, fileMngr, taxonNames );
  //          readQualityScores( userSettings, myQualityScores, fileMngr ); //To Implement
		// set up the phylogenetic model
            Model myModel( &userSettings, myAlignments, taxonNames ); 
	//	Model myModel( &userSettings, myAlignments, myQualityScores, taxonNames ); //To Change

		// run the Markov chain Monte Carlo analysis
		Mcmc myChain( &userSettings, &myModel );
		}
	else 
		{
		// simulate data alignments
		if ( MPI::COMM_WORLD.Get_rank() == 0 )	
			Model myModel( &userSettings, myAlignments );
		MPI::COMM_WORLD.Barrier();
		}
		

	// fare well message
	printFarewell();
		
	// clean up
	for (std::vector<Alignment *>::iterator p=myAlignments.begin(); p != myAlignments.end(); p++)
		delete (*p);

	// terminate MPI
	MPI::Finalize();

    return 0;
}

bool initialize(int& argc, char *argv[]) {

	int processId = 0, numProcesses = 0;
	try {
		MPI::Init(argc, argv);
		processId    = MPI::COMM_WORLD.Get_rank();
		numProcesses = MPI::COMM_WORLD.Get_size();
		}
	catch (char* str)
		{
		return false;
		}
	if ( numProcesses < 2 )
		{
		Msg::error("Too few processes. There must be at least two.");
		return false;
		}
	return true;
}

void initializeSeed(void) {

	// call the seed factory for the first time, which instantiates the factory
	// for each processor (and calls the constructor too)
	SeedFactory& sf = SeedFactory::seedFactoryInstance();
	MPI::COMM_WORLD.Barrier();
}

void printFarewell(void) {

	if ( MPI::COMM_WORLD.Get_rank() == 0 )
		{
		std::cout << "   Successfully ran HDPP analysis" << std::endl;
		std::cout << std::endl;
		}
}

void printGreeting(void) {

	if ( MPI::COMM_WORLD.Get_rank() == 0 )
		{
		std::cout << std::endl;
		std::cout << "   HDPP 2.0" << std::endl;
		std::cout << "   John P. Huelsenbeck, Brian Moore, and Jeremy Brown" << std::endl;
		std::cout << "   University of California, Berkeley" << std::endl;
		std::cout << "   Running with " << MPI::COMM_WORLD.Get_size() << " processors" << std::endl;
		std::cout << std::endl;
		}
}

void readAlignments(Settings& userSettings, std::vector<Alignment*> &alns, IoManager &fileMngr, std::vector<std::string>& taxonNames) {

	// find the process id and the number of processes
	int processId    = MPI::COMM_WORLD.Get_rank();
	int numProcesses = MPI::COMM_WORLD.Get_size();

	// read all of the alignments, calculating the number of nucleotides for each
	int numAlignmentsToRead = 0;
	for (int fn=0; fn<fileMngr.getNumFilesInDirectory(); fn++)
		{
		std::string fileName = fileMngr.getFileNumber(fn);
		Alignment *alignmentPtr = new Alignment( userSettings.getPathName() + "/" + fileName );
		if (fn == 0)
			{
			alignmentPtr->copyTaxonNames(taxonNames);
			}
		else 
			{
			std::vector<std::string>& namesFromAln = alignmentPtr->getTaxonNames();
			bool isSame = true;
			if ( namesFromAln.size() != taxonNames.size() )
				isSame = false;
			else 
				{
				for (int i=0; i<taxonNames.size(); i++)
					if ( namesFromAln[i] != taxonNames[i] )
						isSame = false;
				}
			if (isSame == false)
				{
				for (int i=0; i<taxonNames.size(); i++)
					std::cout << taxonNames[i] << " ";
				std::cout << std::endl;
				for (int i=0; i<namesFromAln.size(); i++)
					std::cout << namesFromAln[i] << " ";
				std::cout << std::endl;
				Msg::error("Missmatch in the taxon names of the alignments: " + alignmentPtr->getFileName());
				}
			}
		numAlignmentsToRead++;
		delete alignmentPtr;
		}
		
	if (processId == 0)
		{
		std::cout << "   Alignments:"                                                                      << std::endl;
		std::cout << "   * Number of alignments                  = " << numAlignmentsToRead                << std::endl;
		std::cout << "   * Number of taxa in each alignment      = " << taxonNames.size()                  << std::endl;
		std::cout << std::endl;
		}
			
	// check that there is at least one alignment per processor
	if ( numAlignmentsToRead < numProcesses )
		Msg::error("Too few alignments for the number of processes");
	
	// divide up the alignments among processes, remembering that process 0 doesn't have any alignments
	int numAlignmentsPerProcessor = numAlignmentsToRead / numProcesses;
	int firstAlignmentId = processId * numAlignmentsPerProcessor;
	int lastAlignmentId  = (processId+1) * numAlignmentsPerProcessor - 1;
	if (lastAlignmentId > numAlignmentsToRead || processId+1 == numProcesses)
		lastAlignmentId = numAlignmentsToRead - 1;
	//std::cout << "Process " << processId << " : " << firstAlignmentId << "-" << lastAlignmentId << std::endl;
	
	// read in the alignments
	for (int fn=0, n=0; fn<fileMngr.getNumFilesInDirectory(); fn++)
		{
		std::string fileName = fileMngr.getFileNumber(fn);
		if (fileName.find(".fasta") == std::string::npos) {
			Msg::error( "File " + fileName + " should end with .fasta");
		}
			std::string qualityScoreFileName = fileName;
		qualityScoreFileName.replace(fileName.find(".fasta"), 6, ".qscores");
		if (n >= firstAlignmentId && n <= lastAlignmentId)
			{
			Alignment *alignmentPtr = new Alignment( userSettings.getPathName() + "/" + fileName );
			QualityScores *qualityScoresPtr = new QualityScores( userSettings.getQualityScorePathName() + "/" + qualityScoreFileName);
			int total = qualityScoresPtr->getNumQualityScores();
			for (int pos = 0; pos < total; pos++) {
			  alignmentPtr->setQualityScore(n, pos, qualityScoresPtr->getQualityScore(pos));
			}
			alignmentPtr->setIndex(n);
			alns.push_back( alignmentPtr );
			}
		n++;
		}
		
#	if 0
	// calculate the number of sites this processor is responsible for
	int numSitesOnProcess = 0;
	for (std::vector<Alignment*>::iterator a=alns.begin(); a != alns.end(); a++)
		numSitesOnProcess += (*a)->getNumChar();
		
	// print list of alignments
	for (std::vector<Alignment*>::iterator a=alns.begin(); a != alns.end(); a++)
		std::cout << processId << " -- " << (*a)->getFileName() << " (" << (*a)->getNumChar() << " " << numSitesOnProcess << ")" << std::endl;
#	endif
}
/*
void readQualityScores(Settings& userSettings, std::vector<QualityScores *> &qss, IoManager &fileMngr) {

  // read in quality scores
  for (int fn = 0; fn<fileMngr.getNumFilesInDirectory(); fn++)
    {
      std::string fileName = fileMngr.getFileNumber(fn);
      QualityScores *qualityScorePtr = new QualityScores(userSettings.getPathName() + "/" + fileName);
      qss.push_back(qualityScorePtr);
    }
}

void associate(std::vector<Alignment*> &alns, std::vector<QualityScores*> &qss) {

  for (int k = 0; k < alns.size(); k++) {
    Alignment currentAln = alns[k];
    QualityScores currentQS = qss[k];
    int total = currentQS.getNumQualityScores();
    for (int j = 0; j < total; j++) {
      currentAln.setQualityScore(0, j, currentQS.getQualityScore(j));
    }
  }
}
*/