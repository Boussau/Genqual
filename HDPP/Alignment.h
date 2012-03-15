#ifndef Alignment_H
#define Alignment_H

#include <string>
#include <vector>

class MbBitfield;

class Alignment {

	public:
                            Alignment(std::string fn);
							Alignment(int nt, int nc, std::string fn);
						   ~Alignment(void);
					 void   copyTaxonNames(std::vector<std::string>& nms);
			  std::string   getFileName(void) { return fileWithAlignment; }
				   double   getIncompleteness(void);
					  int   getIndex(void) { return index; }
			          int   getNucleotide(int tIdx, int sIdx) { return matrix[tIdx][sIdx]; }
			          int   getNumChar(void) { return numChar; }
					  int   getNumMissingEntries(void) { return numMissingEntries; }
					  int   getNumTaxa(void) { return numTaxa; }
                     void   getPossibleNucs(int nucCode, int nuc[]);
	               double   getQualityScore(int tIdx, int cIdx) { return qualityScores[tIdx][cIdx]; }
			  std::string   getTaxonName(int i) { return taxonNames[i]; }
std::vector<std::string>&   getTaxonNames(void) { return taxonNames; }
					 void   print(void);
					 void   setIndex(int x) { index = x; }
	                 void   setQualityScore(int tIdx, int cIdx, double x) { qualityScores[tIdx][cIdx] = x; }

	private:
			          int   numTaxa;
					  int   numChar;
					int**   matrix;
		     	 double**   qualityScores;
 std::vector<std::string>   taxonNames;
					  int   numMissingEntries;
			  std::string   fileWithAlignment;
		              int   nucID(char nuc);    // converts a nucleotide character code to an integer (binary) representation
					  int   index;		    
};

#endif