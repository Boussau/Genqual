//
//  QualityScores.cpp
//  hdpp3
//
//  Created by Bastien Boussau on 16/09/11.
//  Copyright 2011 UC Berkeley. All rights reserved.
//

#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <istream>
#include "MbBitfield.h"
#include "QualityScores.h"
#include <fstream>
#include <map>
#include <math.h>

QualityScores::QualityScores(std::string fn) {

  // initialize the file name (removing the path information and any information after a "."
  std::string tempName = fn;
  int location = tempName.rfind("/");
  if (location != std::string::npos)
    {
      tempName = tempName.substr(location+1, tempName.size());
    }
  location = tempName.find(".");
  if (location != std::string::npos)
    {
      tempName = tempName.substr(0, location);
    }
  fileWithQualityScores = tempName;

  // open the file
  std::ifstream seqStream( fn.c_str() );
  if (!seqStream)
    {
      std::cerr << "Cannot open file \"" + fn + "\"" << std::endl;
      exit(1);
    }
  std::string linestring = "";
  while( getline(seqStream, linestring).good() )
    {
      // remove /n
      linestring = linestring.substr(0, linestring.length() - 1);
      
      // insert scores in scoreVector
      while(linestring.length() > 0)
	{
	  size_t pos = linestring.find(" ");
		std::string substring = "";
		if (pos != std::string::npos)
	    {
	      substring = linestring.substr(0, pos);
	      linestring = linestring.substr(pos + 1);
	    }
	  else
	    {
	      substring = linestring;
	      linestring = "";
	    }
	  scoreVector.push_back(toDouble(substring));
	}
    }

  // close the file
  seqStream.close();

  numQualityScores = scoreVector.size();

  // what's index?
}

double QualityScores::toDouble(const std::string& s)
{
  if (s == "-" || s == "N" || s == "X") {
    return 1.0;
  }
  else {
	std::istringstream iss(s);
	double i;
	iss >> i;
	return (1.0 - (1.0/pow(10.0, (i/10.0))));
  }
}



QualityScores::QualityScores(int nqs, std::string fn) {

	// initialize the file name (removing the path information and any information after a "."
	std::string tempName = fn;
	int location = tempName.rfind("/");
	if (location != std::string::npos)
    {
		tempName = tempName.substr(location+1, tempName.size());
    }
	location = tempName.find(".");
	if (location != std::string::npos)
    {
		tempName = tempName.substr(0, location);
    }
	fileWithQualityScores = tempName;
	
	// open the file
	std::ifstream seqStream( fn.c_str() );
	if (!seqStream)
    {
		std::cerr << "Cannot open file \"" + fn + "\"" << std::endl;
		exit(1);
    }
	std::string linestring = "";
	scoreVector = std::vector<double>(nqs);
	while( getline(seqStream, linestring).good() )
    {
		// remove /n
		linestring = linestring.substr(0, linestring.length() - 1);
		
		// insert scores in scoreVector
		while(linestring.length() > 0)
		{
			size_t pos = linestring.find(" ");
			std::string substring = "";
			if (pos != std::string::npos)
			{
				substring = linestring.substr(0, pos);
				linestring = linestring.substr(pos + 1);
			}
			else
			{
				substring = linestring;
				linestring = "";
			}
			scoreVector[pos] = toDouble(substring);
			pos = pos + 1;
		}
    }
	
	// close the file
	seqStream.close();
	
	numQualityScores = nqs;
	
	// what's index?
}


QualityScores::~QualityScores(void) {
	if (!scoreVector.empty())
	{
		scoreVector.clear();
		numQualityScores = 0;
		fileWithQualityScores = "";
		index = 0;
	}
}


void QualityScores::print(void) {

  for (int k = 0; k < numQualityScores; k++) {
    std::cout << std::setw(4) << scoreVector[k] << " ";
  }
  std::cout << std::endl;

}
