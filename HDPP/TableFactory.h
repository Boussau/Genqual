#ifndef TableFactory_H
#define TableFactory_H

#include <set>
#include <vector>

class Table;

class TableFactory {

	public:
	 static TableFactory&   tableFactoryInstance(void) 
								{
								static TableFactory singleTableFactory;
								return singleTableFactory;
								}
	                 void   drainTablePool(void);
	               Table*   getTable(void);
	                 void   returnTableToPool(Table* t);
	
	private:
							TableFactory(void);                            
							TableFactory(const TableFactory&);            
							TableFactory& operator=(const TableFactory&);
						   ~TableFactory(void);
					  int   processId;
	  std::vector<Table*>   tablePool;
		 std::set<Table*>   allocatedTables;
};

#endif

