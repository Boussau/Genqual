#ifndef ParmKappa_H
#define ParmKappa_H

#include "Parm.h"

class MbRandom;
class MenuItem;
class Model;

class Kappa {

	public:
                            Kappa(MbRandom *rp);
							Kappa(Kappa &k);
                           ~Kappa(void);
			        Kappa   &operator=(const Kappa &k);
					 void   clone(const Kappa &k);
				   double   lnPriorProb(void);
					 void   print(void);
					 void   setKappaFromPrior(void);
				   double   update(void);
				   double   getVal(void) { return kappa; }

	private:
				MbRandom*   ranPtr;
		           double   kappa;
};

class ParmKappa : public Parm {

	public:
                            ParmKappa(Model *mp, MenuItem *ip, long int initSeed, std::string pn);
						   ~ParmKappa(void);
				   double   drawFromPrior(void);
			         void   getParmString(std::string& s);
					 void   keepUpdate(void);
				   double   lnPriorProb(void);
					 void   print(void);
					 void   restore(void);
				   double   update(void);
				   Kappa*   getActiveKappa(void) { return kappa[activeParm]; }

	private:
				    Kappa   *kappa[2];
};

#endif