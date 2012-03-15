#ifndef SiteModels_H
#define SiteModels_H

#include "Parm_key.h"
#include <vector>

class Parm;
class ParmLength;
class ParmSubRates;
class ParmKey;
class ParmStateFreqs;
class ParmTree;

class SiteModels {

	public:
                            SiteModels(int n);
						   ~SiteModels(void);
			  ParmLength*   getLength(int i) { return siteInfo[i].getLength(); }
			ParmSubRates*   getSubRates(int i) { return siteInfo[i].getSubRates(); }
		  ParmStateFreqs*   getStateFreqs(int i) { return siteInfo[i].getStateFreqs(); }
		             void   getSitesWithParm(Parm* p, int parmType, std::vector<int>& siteIndices);
		        ParmTree*   getTree(int i) { return siteInfo[i].getTree(); }
				     void   setLength(int i, ParmLength* p) { siteInfo[i].setLength(p); }
					 void   setSubRates(int i, ParmSubRates* p) { siteInfo[i].setSubRates(p); }
					 void   setParmKey(int i, ParmKey& k);
					 void   setParm(int i, Parm* p, int parmType);
					 void   setStateFreqs(int i, ParmStateFreqs* p) { siteInfo[i].setStateFreqs(p); }
					 void   setTree (int i, ParmTree* p) { siteInfo[i].setTree(p); }
					 void   print(void);

	private:
					  int   numSites;
		         ParmKey*   siteInfo;
};

#endif