#ifndef ParmDnds_H
#define ParmDnds_H

#include "Parm.h"

class MbRandom;
class MenuItem;
class Model;

class Dnds {

	public:
                            Dnds(MbRandom *rp);
							Dnds(Dnds &d);
						   ~Dnds(void);
			         Dnds   &operator=(const Dnds &d);
					 void   clone(const Dnds &d);
					 void   setDndsFromPrior(void);
				   double   lnPriorProb(void);
					 void   print(void);
				   double   update(void);
				   double   getVal(void) { return dnds; }

	private:
				MbRandom*   ranPtr;
		           double   dnds;
};

class ParmDnds : public Parm {

	public:
                            ParmDnds(Model *mp, MenuItem *ip, long int initSeed, std::string pn);
						   ~ParmDnds(void);
				   double   drawFromPrior(void);
			         void   getParmString(std::string& s);
					 void   keepUpdate(void);
				   double   lnPriorProb(void);
					 void   print(void);
					 void   restore(void);
				   double   update(void);
					Dnds*   getActiveDnds(void) { return dnds[activeParm]; }

	private:
	                 Dnds   *dnds[2];
};

#endif