#ifndef RateMatrix_H
#define RateMatrix_H

#include "MbMatrix.h"
#include "Parm_key.h"
#include <set>
#include <string>

class Parm;
class ParmLength;
class ParmSubRates;
class RateMatrixMngr;
class ParmStateFreqs;
class MbTransitionMatrix;
class ParmTree;

class RateMatrix {

	public:
                            RateMatrix(ParmTree *t, ParmStateFreqs *f, ParmSubRates *r, ParmLength *l, int n, RateMatrixMngr *mp);
						   ~RateMatrix(void);
					 void   allocateTransitionProbabilities(void);
					 void   flipActiveQ(void) { (activeQ == 0 ? activeQ = 1 : activeQ = 0); }
				     void   flipActiveTi(void) { (activeTi == 0 ? activeTi = 1 : activeTi = 0); }
					  int   getNumStates(void) { return numStates; }
				 ParmKey*   getParameterKey(void) { return parameterKey; }
		MbMatrix<double>&   getRateMatrix(void) { return q[activeQ]; }
		  ParmStateFreqs*   getStateFreqsPtr(void) { return stateFreqsPtr; }
		MbMatrix<double>&   getTiMatrix(int i) { return tiProb[activeTi][i]; }
		MbMatrix<double>&   getTiMatrix(int space, int i) { return tiProb[space][i]; }
			    ParmTree*   getTreePtr(void) { return treePtr; }
		MbMatrix<double>&   getUniformizedRateMatrix(double* mu);
		             bool   getUpdateFlag(void) { return updateFlag; }
					 void   print(void);
					 void   printTis(void);
					 void   restoreRateMatrix(Parm* p);
					 void   setUpdateFlag(bool tf) { updateFlag = tf; }
					 void   setParmPtrs(ParmTree* t, ParmStateFreqs* f, ParmSubRates* r, ParmLength* l);
					 void   setSubRatesPtr(ParmSubRates* r);
					 void   setLengthPtr(ParmLength* l);
					 void   setRateMatrix(void);
					 void   setStateFreqsPtr(ParmStateFreqs* f);
					 void   setTreePtr(ParmTree* t);
					 void   updateRateMatrix(void);
					 void   updateRateMatrix(Parm* p);
					 void   updateTransitionProbabilities(void);

	private:
		              int   activeQ;
					  int   activeTi;
			  ParmLength*   lenPtr;
			ParmSubRates*   srPtr;
					  int   numNodes;
				      int   numStates;
				 ParmKey*   parameterKey;
		 MbMatrix<double>   q[2];
		  RateMatrixMngr*   rateMatrixMngrPtr;
		  ParmStateFreqs*   stateFreqsPtr;
	  MbTransitionMatrix*   ti;
		MbMatrix<double>*   tiProb[2];
				ParmTree*   treePtr;
		             bool   updateFlag;
};

#endif