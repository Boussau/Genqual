#ifndef TableImage_H
#define TableImage_H

class TableImage {

	public:
                            TableImage(void);
                            TableImage(int r, int i);
					 void   assignToMenuItem(int idx) { indexOfAssignedMenuItem = idx; }
					 void   clean(void);
					  int   getIndexOfAssignedMenuItem(void) { return indexOfAssignedMenuItem; }
				      int   getTableIndex(void) { return tableIndex; }
					  int   getRestaurantIndex(void) { return restaurantIndex; }
					 void   print(void);
					 void   setRestaurantIndex(int x) { restaurantIndex = x; }
					 void   setTableIndex(int x) { tableIndex = x; }

	protected:
					  int   restaurantIndex;
				      int   tableIndex;
					  int   indexOfAssignedMenuItem;
};

#endif