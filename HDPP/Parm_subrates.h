#ifndef ParmSubRates_H
#define ParmSubRates_H

#include "Parm.h"
#include <vector>

class MbRandom;
class MenuItem;
class Model;

class SubRates {

	public:
                            SubRates(MbRandom* rp, double a);
							SubRates(SubRates &f);
						   ~SubRates(void);
			     SubRates   &operator=(const SubRates &f);
					 void   clone(const SubRates &f);
				   double   lnPriorProb(void);
					 void   print(void);
				   double   update(void);
					 void   setSubRatesFromPrior(void);
	 std::vector<double>&   getVal(void) { return rates; }

	private:
				MbRandom*   ranPtr;
	               double   normalizeRates(std::vector<double> &a, double minVal, double total);
	  std::vector<double>   rates;
	  std::vector<double>   dirParm;
				   double   alpha;
};

class ParmSubRates : public Parm {

	public:
                            ParmSubRates(Model* mp, MenuItem* ip, long int initSeed, std::string pn, double a);
						   ~ParmSubRates(void);
				   double   drawFromPrior(void);
			         void   getParmString(std::string& s);
					 void   keepUpdate(void);
				   double   lnPriorProb(void);
					 void   print(void);
					 void   restore(void);
				   double   update(void);
			    SubRates*   getActiveSubRates(void) { return sr[activeParm]; }

	private:
			     SubRates   *sr[2];
};

#endif