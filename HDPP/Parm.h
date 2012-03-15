#ifndef Parm_H
#define Parm_H

#include <string>

class MbRandom;
class MenuItem;
class Model;

class Parm {

	public:
                            Parm(Model *mp, MenuItem *ip, long int initSeed, std::string pn);
				  virtual  ~Parm(void);
					 void   flipActiveParm(void) { (activeParm == 0 ? activeParm = 1 : activeParm = 0); }
					  int   flip(int x);
				MenuItem*   getAssignedMenuItem(void) { return assignedMenuItem; }
			     long int   getInitialSeed(void) { return initialSeed; }
			  std::string   getParmName(void) { return parmName; }
			 virtual void   getParmString(std::string& s)=0;
			 virtual void   keepUpdate(void)=0;
		   virtual double   lnPriorProb(void)=0;
		   virtual double   drawFromPrior(void)=0;
		     virtual void   print(void)=0;
		     virtual void   restore(void)=0;
		   virtual double   update(void)=0;

	protected:
	                  int   activeParm;
				   Model*   modelPtr;
				MenuItem*   assignedMenuItem;
			  std::string   parmName;
				MbRandom*   ranPtr;
				 long int   initialSeed;
};

#endif