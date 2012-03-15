#include <iomanip>
#include <iostream>
#include "Msg.h"
#include "Parm.h"
#include "Parm_length.h"
#include "Parm_subrates.h"
#include "Parm_key.h"
#include "Parm_statefreqs.h"
#include "Parm_tree.h"



ParmKey::ParmKey(void) {

	tree       = NULL;
	length     = NULL;
	subRates   = NULL;
	stateFreqs = NULL;
}

ParmKey::ParmKey(ParmTree* t, ParmLength* l, ParmSubRates* r, ParmStateFreqs* f) {

	tree       = t;
	length     = l;
	subRates   = r;
	stateFreqs = f;
}

ParmKey::ParmKey(const ParmKey& a) {

	tree       = a.tree;
	length     = a.length;
	subRates   = a.subRates;
	stateFreqs = a.stateFreqs;
}

ParmKey::~ParmKey(void) {

}

ParmKey& ParmKey::operator=(const ParmKey& a) {

	if (this != &a)
		{
		tree       = a.tree;
		length     = a.length;
		subRates   = a.subRates;
		stateFreqs = a.stateFreqs;
		}
	return *this;
}

bool ParmKey::operator==(const ParmKey& a) const {

	if ( tree == a.tree && length == a.length && subRates == a.subRates && stateFreqs == a.stateFreqs )
		return true;
	return false;
}

bool ParmKey::operator<(const ParmKey& a) const {

	if ( a.tree < tree )
		return true;
	else if ( a.tree == tree )
		{
		if ( a.length < length )
			return true;
		else if ( a.length == length )
			{
			if ( a.subRates < subRates )
				return true;
			else if ( a.subRates == subRates )
				{
				if ( a.stateFreqs < stateFreqs )
					return true;				
				}
			}
		}
	return false;
}

void ParmKey::print(int i ) {

	std::cout << std::setw(4) << i << " -- t" << tree << " l" << length << " r" << subRates << " f" << stateFreqs << std::endl;
}

void ParmKey::replaceParameterUsing(Parm* p) {

	ParmLength* lenPtr = dynamic_cast<ParmLength *>(p);
	if ( lenPtr == 0 )
		{
		ParmSubRates* srPtr = dynamic_cast<ParmSubRates *>(p);
		if ( srPtr == 0 )
			{
			ParmStateFreqs* stateFreqsPtr = dynamic_cast<ParmStateFreqs *>(p);
			if ( stateFreqsPtr == 0 )
				{
				ParmTree* treePtr = dynamic_cast<ParmTree *>(p);
				if ( stateFreqsPtr == 0 )
					{
					Msg::error("Failed to downcast parameter");
					}
				else 
					{
					setTree(treePtr);
					}
				}
			else 
				{
				setStateFreqs(stateFreqsPtr);
				}
			}
		else 
			{
			setSubRates(srPtr);
			}
		}
	else 
		{
		setLength(lenPtr);
		}

}

void ParmKey::setKey(ParmTree* t, ParmLength* l, ParmSubRates* r, ParmStateFreqs* f) {

	tree       = t;
	length     = l;
	subRates   = r;
	stateFreqs = f;
}
