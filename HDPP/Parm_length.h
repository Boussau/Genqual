#ifndef ParmLength_H
#define ParmLength_H

#include "Parm.h"

class MbRandom;
class MenuItem;
class Model;

class Length {

	public:
                            Length(MbRandom *rp, int nt, double lam);
							Length(Length &k);
                           ~Length(void);
				   Length   &operator=(const Length &k);
					 void   clone(const Length &k);
				   double   lnPriorProb(void);
					 void   print(void);
					 void   setLengthFromPrior(void);
				   double   update(void);
				   double   getVal(void) { return length; }

	private:
				MbRandom*   ranPtr;
		           double   length;
				   double   alpha;
				   double   beta;
};

class ParmLength : public Parm {

	public:
                            ParmLength(Model *mp, MenuItem *ip, long int initSeed, std::string pn, int nt, double lam);
						   ~ParmLength(void);
				   double   drawFromPrior(void);
			         void   getParmString(std::string& s);
					 void   keepUpdate(void);
				   double   lnPriorProb(void);
					 void   print(void);
					 void   restore(void);
				   double   update(void);
				  Length*   getActiveLength(void) { return len[activeParm]; }

	private:
				   Length   *len[2];
};

#endif