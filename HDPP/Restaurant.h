#ifndef Restaurant_H
#define Restaurant_H

#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>

class Alignment;
class CondLikes;
class Franchise;
class FranchiseHq;
class MbRandom;
class RateMatrix;
class RateMatrixMngr;
class Settings;
class SiteLikes;
class SiteModels;
class Table;

class Restaurant {

	public:
                            Restaurant(MbRandom* rp, Settings* sp, Alignment* aln, Franchise* fp, double eK, int pt);
						   ~Restaurant(void);
			       Table*   addTable(void);
				     void   addTable(Table* t);
			   Alignment*   getAlignmentPtr(void) { return alignmentPtr; }
				   double   getAlpha(void) { return alpha; }
				      int   getIndex(void) { return index; }
					  int   getNumPatrons(void) { return numPatrons; }
					  int   getNumTables(void) { return tables.size(); }
		  RateMatrixMngr*   getRateMatrixMngrPtr(void) { return rateMatrixMngrPtr; }
			   SiteLikes*   getSiteLikesPtr(void) { return siteLikesPtr; }
			  SiteModels*   getSiteModelsPtr(void) { return siteModelsPtr; }
	    std::set<Table*>&   getTables(void) { return tables; }
				   Table*   getTableWithPatron(int patronId);
				   Table*   getTableIndexed(int tIdx);
				     bool   isTableInRestaurant(Table* t);
				   double   lnLikelihood(int& siteIdx);
				   double   lnLikelihood(int& siteIdx, MenuItem* m);
				   double   lnLikelihood(int& siteIdx, RateMatrix* rm);
				     void   print(void);
					 void   removeTable(Table* t);
					 void   sampleStates(int n);
				     void   setCondLikesPtr(CondLikes* p) { condLikesPtr = p; }
					 void   setIndex(int x) { index = x; }
				   double   setLnLikelihood(std::vector<int>& siteIndices);
					 void   setRateMatrixMngrPtr(RateMatrixMngr* p) { rateMatrixMngrPtr = p; }
					 void   setSiteLikesPtr(SiteLikes* p) { siteLikesPtr = p; }
					 void   setModelInfo(void);
					 void   setSiteModelsPtr(SiteModels* p) { siteModelsPtr = p; }
					 void   updateSeatingForPatrons(FranchiseHq* hq);

	private:
				     void   getMenuItemString(std::string& s);
					 void   getRgfString(std::string& s);
	               double   expNumTables(double a, int n);
				      int   getNextTableIndex(void);
	                 void   initializeRestaurant(void);
				   double   lnLikelihood(int& pIdx, Table* t);
				     void   openFile(void);
				   Table*   pickTableAtRandom(void);
				   Table*   pickTableAtRandom(std::map<Table*,double>& lnProbs);
				   double   setByExpectedNumTables(double target, int n);
	           Alignment*   alignmentPtr;
			   CondLikes*   condLikesPtr;
			   SiteLikes*   siteLikesPtr;
			  SiteModels*   siteModelsPtr;
		  RateMatrixMngr*   rateMatrixMngrPtr;
	            MbRandom*   ranPtr;
				Settings*   settingsPtr;
			   Franchise*   franchisePtr;
                      int   processId;
					  int   parmTreatment;
				   double   alpha;
	     std::set<Table*>   tables;
		    std::set<int>   availableTableIndices;
		              int   numPatrons;
					  int   parmType;
					  int   index;
					  int   numAuxiliary;
			  std::string   outputFileName;
};

#endif