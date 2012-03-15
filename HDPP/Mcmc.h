#ifndef Mcmc_H
#define Mcmc_H

class MbRandom;
class Model;
class Settings;

class Mcmc {

	public:
                            Mcmc(Settings* sp, Model* mp);
						   ~Mcmc(void);

	private:
	                 void   finalizeChain(void);
	                 void   initializeChain(void);
					 void   printToScreen(int n, int franchiseToUpdate);
					 void   runChain(void);
					 void   sampleChain(int n);
                      int   processId;
				   Model*   modelPtr;
	            MbRandom*   universalRanPtr;
				Settings*   settingsPtr;
				      int   chainLength;
					  int   printFrequency;
					  int   sampleFrequency;
};

#endif