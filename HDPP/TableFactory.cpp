#include <iostream>
#include <mpi.h>
#include "Table.h"
#include "TableFactory.h"



TableFactory::TableFactory(void) {

	// who am I?
	processId = MPI::COMM_WORLD.Get_rank();
}

TableFactory::~TableFactory(void) {

	for (std::set<Table*>::iterator t=allocatedTables.begin(); t != allocatedTables.end(); t++)
		delete (*t);
}

void TableFactory::drainTablePool(void) {

	for (std::vector<Table*>::iterator t=tablePool.begin(); t != tablePool.end(); t++)
		{
		allocatedTables.erase( *t );
		delete (*t);
		}
}

Table* TableFactory::getTable(void) { 

	if ( tablePool.empty() == true )
		{
		/* If the node pool is empty, we allocate a new node and return it. We
		   do not need to add it to the node pool. */
		Table *t = new Table;
		allocatedTables.insert( t );
		return t;
		}
		
	// Return a table from the node pool, remembering to remove it from the pool.
	Table *t = tablePool.back();
	tablePool.pop_back();
	return t;
}

void TableFactory::returnTableToPool(Table* t) {

	t->clean();               // Make certain the information in the table is in some base state before returning it to the node pool.
	tablePool.push_back( t );
}
