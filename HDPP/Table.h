#ifndef Table_H
#define Table_H

#include <set>
#include "TableImage.h"

class Table : public TableImage {

	public:
                            Table(void);
						   ~Table(void);
					 void   addPatron(int patronId);
					 void   clean(void);
					  int   getNumPatrons(void) { return patrons.size(); }
		   std::set<int>&   getPatronList(void) { return patrons; }
		           double   getScratchVal(void) { return scratchVal; }
					 bool   isPatronAtTable(int i);
					 void   print(void);
					 void   removePatron(int patronId);
				     void   setScratchVal(double x) { scratchVal = x; }

	private:
	        std::set<int>   patrons;
			       double   scratchVal;
};

#endif