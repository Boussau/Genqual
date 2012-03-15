#ifndef StateSets_H
#define StateSets_H

#include <vector>

class Alignment;
class Settings;
class Node;

class StateSets {

	public:
                            StateSets(Alignment* ap, Settings* sp);
							StateSets(void);
					  int   getIndex(void) { return index; }
					  int   getNumChar(void) { return numChar; }
					  int   getLengthOne(void) { return lengthOne; }
					  int   getLengthTwo(void) { return lengthTwo; }
				unsigned*   getStsPtr(int space, int node) { return stsPtr[space][node]; }
				      int   calcStateSetFor(int s1, int n1, int s2, int n2, int s3, int n3);
					  int   initializeStateSets(std::vector<Node*>& dp);
					 void   setIndex(int x) { index = x; }
					 void   setLengthOne(int x) { lengthOne = x; }
					 void   setLengthTwo(int x) { lengthTwo = x; }

	private:
			   Alignment*   alignmentPtr;
				Settings*   settingsPtr;
				unsigned*   sts;
			  unsigned***   stsPtr;
					int**   lengths;
			   unsigned**   codedMatrix;
			          int   index;
                      int   numTaxa;
					  int   numChar;
					  int   numNodes;
					  int   oneStateSetSize;
					  int   lengthOne;
					  int   lengthTwo;

};

#endif