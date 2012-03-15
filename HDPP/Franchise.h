#ifndef Franchise_H
#define Franchise_H

#define PARM_TREE 0
#define PARM_LENG 1
#define PARM_SUBR 2
#define PARM_FREQ 3

#include <map>
#include <set>
#include <string>
#include <vector>

class Alignment;
class FranchiseHq;
class MbRandom;
class MenuItem;
class Model;
class Restaurant;
class Settings;
class Table;
class TableImage;

class Franchise {

	public:
                            Franchise(MbRandom* rp, Model* mp, Settings* sp, std::vector<Alignment*>& alns, std::string pt, std::vector<std::string>& tn);
						   ~Franchise(void);
				   Table*   findTable(int rIdx, int tIdx);
			  std::string   getFranchiseName(void) { return franchiseName; }
			 FranchiseHq*   getFranchiseHqPtr(void) { return headQuarters; }
			          int   getNumPatronsInFranchise(void) { return numPatronsInFranchise; }
			  std::string   getParmName(void) { return parmName; }
			          int   getParmTreatment(void) { return parmTreatment; }
					  int   getParmType(void) { return parmType; }
std::vector<Restaurant*>&   getRestaurants(void) { return restaurants; }
                   double   lnLikelihood(MenuItem* m);
                     void   sampleStates(int n);
					 void   synchronizeFranchises(void);
					 void   synchronizeFranchiseHqSeeds(void);
					 void   updateSeating(void);
					 void   updateStickWeights(void);
					 void   updateParameters(void);

	private:
	                 void   calculatePriorStatistics(void);
	                  int   determineParmTreatment(std::string pt);
					  int   determineParmType(std::string pt);
					  int   drawNumberOfTablesInCpp(double a, int n);
					 void   gatherRestaurantTables(std::vector<TableImage*>& tableImages);
					  int   getNumPatronsOnProcess(void);
	                 void   initializeFranchise(std::vector<Alignment*>& alns, double eK);
					 void   initializeFranchiseForSimulation(std::vector<Alignment*>& alns, double eK);
					 void   shareRestaurantTableInformation(std::vector<TableImage*>& tableImages);
                      int   processId;
					  int   numProcesses;
	            MbRandom*   ranPtr;
				   Model*   modelPtr;
				Settings*   settingsPtr;
			  std::string   franchiseName;
			  std::string   parmName;
			          int   numPatronsInFranchise;
				      int   parmType;
					  int   parmTreatment;
					  int   numRestaurants;
			 FranchiseHq*   headQuarters;
 std::vector<Restaurant*>   restaurants;
 std::vector<std::string>   taxonNames;
};

#endif