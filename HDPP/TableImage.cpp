#include <iomanip>
#include <iostream>
#include "TableImage.h"



TableImage::TableImage(void) {

	clean();
}

TableImage::TableImage(int r, int i) {

	restaurantIndex = r;
	tableIndex      = i;
}

void TableImage::clean(void) {

	restaurantIndex         = 0;
	tableIndex              = 0;
	indexOfAssignedMenuItem = 0;
}

void TableImage::print(void) {

	std::cout << std::setw(4) << restaurantIndex << " " << std::setw(4) << tableIndex << " " << std::setw(4) << indexOfAssignedMenuItem << std::endl;
}
