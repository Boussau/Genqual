#include <iomanip>
#include <iostream>
#include "Franchise.h"
#include "Msg.h"
#include "Parm.h"
#include "Parm_length.h"
#include "Parm_subrates.h"
#include "Parm_statefreqs.h"
#include "Parm_tree.h"
#include "SiteLikes.h"
#include "SiteModels.h"



SiteInfo::SiteInfo(void) {

	like[0] = 0.0;
	like[1] = 0.0;
	activeLike = 0;
}

int SiteInfo::flip(int x) {

	if (x == 0)
		return 1;
	return 0;
}

SiteLikes::SiteLikes(int n) {

	numSites = n;
	siteInfo = new SiteInfo[numSites];
}

SiteLikes::~SiteLikes(void) {

	delete [] siteInfo;
}

void SiteLikes::flipActiveLikeForSites(std::vector<int>& affectedSites) {

	for (std::vector<int>::iterator i=affectedSites.begin(); i != affectedSites.end(); i++)
		siteInfo[(*i)].flipActiveLike();
}

void SiteLikes::flipActiveLikeForSitesSharingParm(Parm* p, SiteModels* sm, int parmType) {

	if ( parmType == PARM_TREE )
		{
		ParmTree* t = dynamic_cast<ParmTree*>(p);
		if ( t == 0 )
			Msg::error("Problem in flipActiveLikeForSitesSharingParm");
		for (int i=0; i<numSites; i++)
			{
			if ( sm->getTree(i) == t )
				siteInfo[i].flipActiveLike();
			}
		}
	else if ( parmType == PARM_LENG )
		{
		ParmLength* d = dynamic_cast<ParmLength*>(p);
		if ( d == 0 )
			Msg::error("Problem in flipActiveLikeForSitesSharingParm");
		for (int i=0; i<numSites; i++)
			{
			if ( sm->getLength(i) == d )
				siteInfo[i].flipActiveLike();
			}
		}
	else if ( parmType == PARM_SUBR )
		{
		ParmSubRates* k = dynamic_cast<ParmSubRates*>(p);
		if ( k == 0 )
			Msg::error("Problem in flipActiveLikeForSitesSharingParm");
		for (int i=0; i<numSites; i++)
			{
			if ( sm->getSubRates(i) == k )
				siteInfo[i].flipActiveLike();
			}
		}
	else if ( parmType == PARM_FREQ )
		{
		ParmStateFreqs* f = dynamic_cast<ParmStateFreqs*>(p);
		if ( f == 0 )
			Msg::error("Problem in flipActiveLikeForSitesSharingParm");
		for (int i=0; i<numSites; i++)
			{
			if ( sm->getStateFreqs(i) == f )
				siteInfo[i].flipActiveLike();
			}
		}
	else 
		Msg::error("Unknown parameter type");
}

double SiteLikes::getActiveLike(void) {

	double lnL = 0.0;
	for (int i=0; i<numSites; i++)
		lnL += siteInfo[i].getActiveLike();
	return lnL;
}

void SiteLikes::print(void) {

	double sum = 0.0;
	for (int i=0; i<numSites; i++)
		{
		sum += siteInfo[i].getLnLikeDiff();
		std::cout << std::setw(4) << i << " -- " << std::fixed << std::setprecision(8) << siteInfo[i].getLike(0) << " " << siteInfo[i].getLike(1) << " " << siteInfo[i].getActiveState() << " " << siteInfo[i].getLnLikeDiff() << " " << sum << std::endl;
		}
}
