#ifndef Settings_H
#define Settings_H

#define PARM_DIFF	0
#define PARM_SAME	1
#define PARM_RV		2

#include <string>

class Settings {

	public:
                            Settings(int argc, char *argv[]);
						   ~Settings(void);
					 bool   getAreParametersEstimated(void) { return estimateParms; }
			          int   getBurnIn(void) { return burnIn; }
					  int   getChainLength(void) { return chainLength; }
			  std::string   getPathName(void) { return pathName; }
			  std::string   getQualityScorePathName(void) { return qualityScorePathName;}
				   double   getBrlenLambda(void) { return brlenLambda; }
				      int   getPrespecifiedNumberOfTaxa(void) { return specifiedNumTaxa; }
				   double   getMenuAlpha(void) { return menuAlpha; }
				      int   getNumAuxiliaryMenuItems(void) { return numAuxiliaryMenuItems; }
					  int   getNumSimulatedRestaurants(void) { return numSimulatedRestaurants; }
					  int   getNumSimulatedSites(void) { return numSimulatedSites; }
					  int   getNumSimulatedTaxa(void) { return numSimulatedTaxa; }
				   double   getPriorMeanRest(void) { return ekRestaurant; }
				      int   getPrintFrequency(void) { return printFrequency; }
			  std::string   getOutputFileName(void) { return outputFileName; }
			          int   getSampleFrequency(void) { return sampleFrequency; }
				      int   getTreatmentTopology(void) { return treatmentTopology; }
					  int   getTreatmentBasefreqs(void) { return treatmentBasefreqs; }
					  int   getTreatmentSubRates(void) { return treatmentSubRates; }
					  int   getTreatmentLength(void) { return treatmentLength; }
					 void   print(void);
					 bool   shouldAlignmentsHaveEqualNumberOfTaxa(void) { return equalNumTaxa; }

	private:
			  std::string   pathName;
			  std::string	qualityScorePathName;
			  std::string   outputFileName;
				   double   brlenLambda;
				   double   menuAlpha;
				   double   ekRestaurant;
				      int   treatmentTopology;
					  int   treatmentBasefreqs;
					  int   treatmentSubRates;
					  int   treatmentLength;	
					  int   chainLength;
					  int   printFrequency;
					  int   sampleFrequency;
					  int   burnIn;	
					 bool   equalNumTaxa;
					  int   specifiedNumTaxa;
					  int   numAuxiliaryMenuItems;
					 bool   estimateParms;
					  int   numSimulatedTaxa;
					  int   numSimulatedSites;
					  int   numSimulatedRestaurants;
};

#endif