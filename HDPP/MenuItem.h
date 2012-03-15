#ifndef MenuItem_H
#define MenuItem_H

#include <set>
#include <string>
#include <vector>
#include "TableImage.h"

class MbRandom;
class Model;
class Parm;
class Settings;

class MenuItem {

	public:
                            MenuItem(MbRandom* rp, Model* mp, Settings* sp, int pt, std::vector<std::string> tn);
							MenuItem(MbRandom* rp, Model* mp, Settings* sp, int pt, std::vector<std::string> tn, long int initSeed);
						   ~MenuItem(void);
                     void   addTableImage(TableImage* t);
					 void   clearTableImages(void);
			  TableImage*   findTableImage(int& rIdx, int& tIdx);
					  int   getIndex(void) { return index; }
					  int   getInitialParameterSeed(void);
					  int   getNumTableImages(void) { return tableImages.size(); }
					  int   getParmType(void) { return parmType; }
					Parm*   getParameterPtr(void) { return parameter; }
			  std::string   getParameterString(void);
				   double   getStickLength(void) { return stickLength; }
   std::set<TableImage*>&   getTableImages(void) { return tableImages; }
                     void   keepUpdate(void);
                   double   lnPriorProbability(void);
					 void   removeAllTableImages(void);
                     void   removeTableImage(TableImage* t);
                     void   removeTableImage(int rIdx, int tIdx);
                     void   setIndex(int x) { index = x; }
				     void   setStickLength(double x) { stickLength = x; }
				     bool   isTableAssignedToMenuItem(int rIdx, int tIdx);
					 void   print(void);
					 void   restore(void);
				   double   updateParameter(void);

	private:
	                 void   initializeMenuItem(std::vector<std::string> tn, long int initSeed);
	                  int   index;
				   Model*   modelPtr;
		            Parm*   parameter;
				      int   parmType;
	            MbRandom*   ranPtr;
				   double   stickLength;
				Settings*   settingsPtr;
    std::set<TableImage*>   tableImages;
};

class comp_menuItem {

	public:
                     bool   operator()(MenuItem *m1, MenuItem *m2) const { return (m1->getIndex() < m2->getIndex()); }
};

#endif