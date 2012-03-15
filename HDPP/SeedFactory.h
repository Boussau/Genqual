#ifndef SeedFactory_H
#define SeedFactory_H

class MbRandom;

class SeedFactory {

	public:
	  static SeedFactory&   seedFactoryInstance(void) 
								{
								static SeedFactory singleSeedFactory;
								return singleSeedFactory;
								}
		         long int   getLocalSeed(void);
		         long int   getUniversalSeed(void);
		
	private:
		                    SeedFactory(void);
		                    SeedFactory(const SeedFactory&);
		                    SeedFactory& operator=(const SeedFactory&);
		                   ~SeedFactory(void);
					  int   processId;
		        MbRandom*   localSeedGenerator;
		        MbRandom*   universalSeedGenerator;
};

#endif

