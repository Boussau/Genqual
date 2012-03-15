#include "Table.h"
#include <iomanip>
#include <iostream>



Table::Table(void) {

	clean();
}

Table::~Table(void) {

}

void Table::addPatron(int patronId) {

	patrons.insert( patronId );
}

void Table::clean(void) {

	patrons.clear();
	tableIndex              = 0;
	restaurantIndex         = 0;
	indexOfAssignedMenuItem = 0;
	scratchVal              = 0.0;
}

bool Table::isPatronAtTable(int i) {
	
	std::set<int>::iterator it = patrons.find( i );
	if ( it != patrons.end() )
		return true;
	return false;
}

void Table::print(void) {

	std::cout << std::setw(4) << restaurantIndex << std::setw(4) << tableIndex << " -- ";
	int i = 0;
	for (std::set<int>::iterator p=patrons.begin(); p != patrons.end(); p++)
		{
		std::cout << std::setw(3) << (*p) << " ";
		i++;
		if (i % 40 == 0)
			{
			std::cout << std::endl;
			if ( i+1 != patrons.size() )
				std::cout << "         -- ";
			}
		}
	std::cout << std::endl;
}

void Table::removePatron(int patronId) {

	patrons.erase( patronId );
}
