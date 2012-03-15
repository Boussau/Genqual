#ifndef SITE_KEY_H
#define SITE_KEY_H

class SiteKey {

	public:
                                   SiteKey(int a, int s);
							       SiteKey(const SiteKey &a);
								   ~SiteKey(void);
					         int   getAlignmentIdx(void) { return alignmentIdx; }
					         int   getSiteIdx(void) { return siteIdx; }
                            bool   operator==(const SiteKey &a) const { return (alignmentIdx == a.alignmentIdx && siteIdx == a.siteIdx); }
                            bool   operator<(const SiteKey &a) const { return (alignmentIdx < a.alignmentIdx || (alignmentIdx == a.alignmentIdx && siteIdx < a.siteIdx)); }
							void   setAlignmentIdx(int x) { alignmentIdx = x; }
							void   setSiteIdx(int x) { siteIdx = x; }

	private:
	                         int   alignmentIdx;
							 int   siteIdx;
};

#endif


