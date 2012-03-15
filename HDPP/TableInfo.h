#ifndef TableInfo_H
#define TableInfo_H

class TableInfo {

	public:
                            TableInfo(int ri, int ti, int mi);
					  int   getRestaurantIndex(void) { return restaurantIndex; }
					  int   getTableIndex(void) { return tableIndex; }
					  int   getMenuItemIndex(void) { return menuItemIndex; }

	private:
	                  int   restaurantIndex;
				      int   tableIndex;
					  int   menuItemIndex;
};

#endif