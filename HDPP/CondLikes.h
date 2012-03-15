#ifndef CondLikes_H
#define CondLikes_H

class Alignment;

class CondLikes {

	public:
                            CondLikes(Alignment* aln);
						   ~CondLikes(void);
				  double*   getClsPtr(int nodeIdx, int siteIdx) { return clsPtr[nodeIdx][siteIdx]; }
					 void   print(void);

	private:
	                 void   initializeCondLikes(Alignment* aln);
			          int   numTaxa;
					  int   numSites;
					  int   numNodes;
					  int   numStates;
					  int   oneNodeSize;
				  double*   cls;
				double***   clsPtr;
};

#endif