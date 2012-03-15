//
//  QualityScores.h
//  hdpp3
//
//  Created by Bastien Boussau on 16/09/11.
//  Copyright 2011 UC Berkeley. All rights reserved.
//

#ifndef QualityScores_h
#define QualityScores_h

#include <string>
#include <vector>

class QualityScores {
    
public:
    QualityScores(std::string fn);
	double toDouble(const std::string& s);
    QualityScores(int nqs, std::string fn);
    ~QualityScores(void);
    std::string   getFileName(void) { return fileWithQualityScores; }
    int   getIndex(void) { return index; }
    double   getQualityScore(double sIdx) { return scoreVector[sIdx]; }
    int   getNumQualityScores(void) { return numQualityScores; }
    void   print(void);
    void   setIndex(int x) { index = x; }
    
private:
    int   numQualityScores;
    std::vector<double>   scoreVector;
    std::string   fileWithQualityScores;
    int   index;		    
};


#endif
