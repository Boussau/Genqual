#ifndef ParmKey_H
#define ParmKey_H

class Parm;
class ParmLength;
class ParmSubRates;
class ParmStateFreqs;
class ParmTree;

class ParmKey {

	public:
	                               ParmKey(void);
                                   ParmKey(ParmTree* t, ParmLength* l, ParmSubRates* r, ParmStateFreqs* f);
							       ParmKey(const ParmKey& a);
								  ~ParmKey(void);
                        ParmKey&   operator=(const ParmKey& a);
                            bool   operator==(const ParmKey& a) const;
                            bool   operator<(const ParmKey& a) const;
					   ParmTree*   getTree(void) { return tree; }
					 ParmLength*   getLength(void) { return length; }
				   ParmSubRates*   getSubRates(void) { return subRates; }
				 ParmStateFreqs*   getStateFreqs(void) { return stateFreqs; }
				            void   print(int i);
							void   replaceParameterUsing(Parm* p);
					        void   setKey(ParmTree *t, ParmLength* l, ParmSubRates* r, ParmStateFreqs* f);
							void   setTree(ParmTree* p) { tree = p; }
							void   setLength(ParmLength* p) { length = p; }
							void   setSubRates(ParmSubRates* p) { subRates = p; }
							void   setStateFreqs(ParmStateFreqs* p) { stateFreqs = p; }

	private:
					   ParmTree*   tree;
					 ParmLength*   length;
				   ParmSubRates*   subRates;
				 ParmStateFreqs*   stateFreqs;
};

#endif