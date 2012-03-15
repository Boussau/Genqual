#ifndef MenuItemInfo_H
#define MenuItemInfo_H

class MenuItemInfo {

	public:
                            MenuItemInfo(int idx, long int ps);
					  int   getIndex(void) { return index; }
				 long int   getInitialParameterSeed(void) { return initialParameterSeed; }

	private:
	                  int   index;
				 long int   initialParameterSeed;
};

#endif