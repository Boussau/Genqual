#ifndef ParmStateFreqs_H
#define ParmStateFreqs_H

#include "Parm.h"
#include <vector>

class MbRandom;
class MenuItem;
class Model;

class StateFreqs {

	public:
                            StateFreqs(MbRandom* rp, int n, double a);
							StateFreqs(StateFreqs &f);
						   ~StateFreqs(void);
			   StateFreqs   &operator=(const StateFreqs &f);
					 void   clone(const StateFreqs &f);
				   double   lnPriorProb(void);
					 void   print(void);
				   double   update(void);
					 void   setStateFreqsFromPrior(void);
	 std::vector<double>&   getVal(void) { return freqs; }

	private:
				MbRandom*   ranPtr;
	               double   normalizeFreqs(std::vector<double> &a, double minVal, double total);
	  std::vector<double>   freqs;
	  std::vector<double>   dirParm;
					  int   numStates;
				   double   alpha;
};

class ParmStateFreqs : public Parm {

	public:
                            ParmStateFreqs(Model* mp, MenuItem* ip, long int initSeed, std::string pn, int n, double a);
						   ~ParmStateFreqs(void);
				   double   drawFromPrior(void);
			         void   getParmString(std::string& s);
					 void   keepUpdate(void);
				   double   lnPriorProb(void);
					 void   print(void);
					 void   restore(void);
				   double   update(void);
			  StateFreqs*   getActiveStateFreqs(void) { return sf[activeParm]; }

	private:
			   StateFreqs   *sf[2];
};

#endif