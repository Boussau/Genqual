#include "SiteKey.h"



SiteKey::SiteKey(int a, int s) {

	alignmentIdx = a;
	siteIdx      = s;
}

SiteKey::SiteKey(const SiteKey &a) {

	alignmentIdx = a.alignmentIdx;
	siteIdx      = a.siteIdx;
}

SiteKey::~SiteKey(void) {

}

