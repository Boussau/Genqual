#include <iostream>
#include <mpi.h>
#include "TableImage.h"
#include "TableImageFactory.h"



TableImageFactory::TableImageFactory(void) {

	// who am I?
	processId = MPI::COMM_WORLD.Get_rank();
}

TableImageFactory::~TableImageFactory(void) {

	for (std::set<TableImage*>::iterator t=allocatedTableImages.begin(); t != allocatedTableImages.end(); t++)
		delete (*t);
}

void TableImageFactory::drainTableImagePool(void) {

	for (std::vector<TableImage*>::iterator t=tableImagePool.begin(); t != tableImagePool.end(); t++)
		{
		allocatedTableImages.erase( *t );
		delete (*t);
		}
}

TableImage* TableImageFactory::getTableImage(void) { 

	if ( tableImagePool.empty() == true )
		{
		/* If the node pool is empty, we allocate a new node and return it. We
		   do not need to add it to the node pool. */
		TableImage *t = new TableImage;
		allocatedTableImages.insert( t );
		return t;
		}
		
	// Return a table from the node pool, remembering to remove it from the pool.
	TableImage *t = tableImagePool.back();
	tableImagePool.pop_back();
	return t;
}

void TableImageFactory::returnTableImageToPool(TableImage* t) {

	t->clean();               // Make certain the information in the table is in some base state before returning it to the node pool.
	tableImagePool.push_back( t );
}

