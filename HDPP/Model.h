#ifndef Model_H
#define Model_H

#include <string>
#include <vector>

class Alignment;
class CondLikes;
class Franchise;
class MbRandom;
class RateMatrixMap;
class RateMatrixMngr;
class Settings;
class SiteLikes;
class SiteModels;
class StateSets;

class Model {

	public:
                                 Model(Settings* sp, std::vector<Alignment*>& alns, std::vector<std::string>& taxonNames);
                                 Model(Settings* sp, std::vector<Alignment*>& alns);
						        ~Model(void);
				        double   calculateGlobalLikelihood(void);
					      void   checkHeadQuartersConsistency(int franchiseToUpdate);
			       std::string   getFranchiseName(int franchiseNum);
					       int   getNumStates(void) { return numStates; }
                    Franchise*   getTreeFranchisePtr(void);
					       int   pickFranchise(void);
					      void   sampleStates(int n);
				          void   updateFranchise(int franchiseToUpdate);

	private:
	                      void   initializeModel(std::vector<Alignment*>& alns, std::vector<std::string>& taxonNames);
	                      void   initializeSimulationModel(std::vector<Alignment*>& alns);
					      void   simulate(void);
                           int   processId;
					       int   numStates;
	                 MbRandom*   localRanPtr;
				     MbRandom*   universalRanPtr;
				     Settings*   settingsPtr;
			       SiteModels*   siteModelsPtr;
       std::vector<Franchise*>   parameters;
           std::vector<double>   proposalWeights;
	       std::vector<double>   proposalProbs;
       std::vector<CondLikes*>   condLikes;
       std::vector<SiteLikes*>   siteLikes;
      std::vector<SiteModels*>   siteModels;
                RateMatrixMap*   rateMatrixMap;
  std::vector<RateMatrixMngr*>   rateMatrixMngrs;
       std::vector<StateSets*>   stateSets;
};

#endif