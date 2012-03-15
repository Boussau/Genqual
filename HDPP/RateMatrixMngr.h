#ifndef RateMatrixMngr_H
#define RateMatrixMngr_H

#include <map>
#include <set>
#include <string>
#include <vector>

class Alignment;
class Model;
class ParmLength;
class ParmSubRates;
class ParmKey;
class ParmStateFreqs;
class ParmTree;
class RateMatrix;

class RateMatrixMngr {

	public:
                                   RateMatrixMngr(Model *mp, int ns);
								  ~RateMatrixMngr(void);
							void   addMatrixToMngr(ParmKey queryKey, RateMatrix* m);
					        void   addMatrixToMngr(ParmTree* t, ParmLength* l, ParmSubRates* r, ParmStateFreqs* f, RateMatrix* m);
			                void   deleteMatrix(RateMatrix* m);
							bool   findDuplicateRateMatrices(std::set<RateMatrix*> &duplicates);
							 int   getNumStates(void) { return numStates; }
					 RateMatrix*   getNewRateMatrix(ParmKey& k);
					 RateMatrix*   getNewRateMatrix(ParmTree* t, ParmLength* l, ParmSubRates* r, ParmStateFreqs* f);
		  std::set<RateMatrix*>&   getMatricesSharingLengthParm(Parm* p);
		  std::set<RateMatrix*>&   getMatricesSharingSubRatesParm(Parm* p);
		  std::set<RateMatrix*>&   getMatricesSharingStateFreqsParm(Parm* p);
		  std::set<RateMatrix*>&   getMatricesSharingTreeParm(Parm* p);
		  std::set<RateMatrix*>&   getMatricesSharingParm(Parm* p, int parmType);
  std::map<ParmKey,RateMatrix*>&   getRateMatrixList(void) { return rateMatrixMap; }
			                void   print(void);
							void   removeMatrixFromMngr(RateMatrix* m);
			         RateMatrix*   retrieveRateMatrix(ParmKey& queryKey);
			         RateMatrix*   retrieveRateMatrix(ParmTree* t, ParmLength* l, ParmSubRates* r, ParmStateFreqs* f);

	private:
						  Model*   modelPtr;
					         int   numStates;
							 int   numSites;
   std::map<ParmKey,RateMatrix*>   rateMatrixMap;
           std::set<RateMatrix*>   rateMatricesSharingParm;
};

#endif

