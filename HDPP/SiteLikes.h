#ifndef SiteLikes_H
#define SiteLikes_H

class Parm;
class SiteModels;

class SiteInfo {

	public:
                            SiteInfo(void);
					  int   getActiveState(void) { return activeLike; }
				   double   getActiveLike(void) { return like[activeLike]; }
				   double   getLike(int i) { return like[i]; }
				   double   getLnLikeDiff(void) { return like[activeLike] - like[flip(activeLike)]; }
					  int   flip(int x);
					 void   flipActiveLike(void) { (activeLike == 0 ? activeLike = 1 : activeLike = 0); }
					 void   setActiveLike(double x) { like[activeLike] = x; }

	private:
					  int   activeLike;
				   double   like[2];
};

class SiteLikes {

	public:
                            SiteLikes(int n);
						   ~SiteLikes(void);
					 void   flipActiveLikeForSites(std::vector<int>& affectedSites);
					 void   flipActiveLikeForSitesSharingParm(Parm* p, SiteModels* sm, int parmType);
				   double   getActiveLike(int siteIdx) { return siteInfo[siteIdx].getActiveLike(); }
				   double   getActiveLike(void);
				   double   getLnDiff(int siteIdx) { return siteInfo[siteIdx].getLnLikeDiff(); }
					 void   print(void);
					 void   setActiveLike(int siteIdx, double x) { siteInfo[siteIdx].setActiveLike(x); }

	private:
					  int   numSites;
				SiteInfo*   siteInfo;
};

#endif