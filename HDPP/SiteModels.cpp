#include <iomanip>
#include <iostream>
#include "Franchise.h"
#include "Msg.h"
#include "Parm_length.h"
#include "Parm_subrates.h"
#include "Parm_key.h"
#include "Parm_statefreqs.h"
#include "Parm_tree.h"
#include "SiteModels.h"



SiteModels::SiteModels(int n) {

	numSites = n;
	siteInfo = new ParmKey[numSites];
}

SiteModels::~SiteModels(void) {

	delete [] siteInfo;
}

void SiteModels::print(void) {

	for (int i=0; i<numSites; i++)
		siteInfo[i].print(i);
}

void SiteModels::getSitesWithParm(Parm* p, int parmType, std::vector<int>& siteIndices) {

	siteIndices.clear();
	
	if ( parmType == PARM_TREE )
		{
		ParmTree* t = dynamic_cast<ParmTree*>(p);
		if ( t == 0 )
			Msg::error("Problem in setParm");
		for (int i=0; i<numSites; i++)
			{
			if ( siteInfo[i].getTree() == t )
				siteIndices.push_back(i);
			}
		}
	else if ( parmType == PARM_LENG )
		{
		ParmLength* d = dynamic_cast<ParmLength*>(p);
		if ( d == 0 )
			Msg::error("Problem in setParm");
		for (int i=0; i<numSites; i++)
			{
			if ( siteInfo[i].getLength() == d )
				siteIndices.push_back(i);
			}
		}
	else if ( parmType == PARM_SUBR )
		{
		ParmSubRates* k = dynamic_cast<ParmSubRates*>(p);
		if ( k == 0 )
			Msg::error("Problem in setParm");
		for (int i=0; i<numSites; i++)
			{
			if ( siteInfo[i].getSubRates() == k )
				siteIndices.push_back(i);
			}
		}
	else if ( parmType == PARM_FREQ )
		{
		ParmStateFreqs* f = dynamic_cast<ParmStateFreqs*>(p);
		if ( f == 0 )
			Msg::error("Problem in setParm");
		for (int i=0; i<numSites; i++)
			{
			if ( siteInfo[i].getStateFreqs() == f )
				siteIndices.push_back(i);
			}
		}
	else 
		Msg::error("Unknown parameter type");
}

void SiteModels::setParm(int i, Parm* p, int parmType) {

	if ( parmType == PARM_TREE )
		{
		ParmTree* t = dynamic_cast<ParmTree*>(p);
		if ( t == 0 )
			Msg::error("Problem in setParm");
		siteInfo[i].setTree(t);
		}
	else if ( parmType == PARM_LENG )
		{
		ParmLength* d = dynamic_cast<ParmLength*>(p);
		if ( d == 0 )
			Msg::error("Problem in setParm");
		siteInfo[i].setLength(d);
		}
	else if ( parmType == PARM_SUBR )
		{
		ParmSubRates* k = dynamic_cast<ParmSubRates*>(p);
		if ( k == 0 )
			Msg::error("Problem in setParm");
		siteInfo[i].setSubRates(k);
		}
	else if ( parmType == PARM_FREQ )
		{
		ParmStateFreqs* f = dynamic_cast<ParmStateFreqs*>(p);
		if ( f == 0 )
			Msg::error("Problem in setParm");
		siteInfo[i].setStateFreqs(f);
		}
	else 
		Msg::error("Unknown parameter type");
}

void SiteModels::setParmKey(int i, ParmKey& k) {

	k.setTree( siteInfo[i].getTree() );
	k.setLength( siteInfo[i].getLength() );
	k.setSubRates( siteInfo[i].getSubRates() );
	k.setStateFreqs( siteInfo[i].getStateFreqs() );
}
