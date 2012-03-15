#ifndef TableImageFactory_H
#define TableImageFactory_H

#include <set>
#include <vector>

class TableImage;

class TableImageFactory {

	public:
static TableImageFactory&   tableImageFactoryInstance(void) 
								{
								static TableImageFactory singleTableImageFactory;
								return singleTableImageFactory;
								}
	                 void   drainTableImagePool(void);
	          TableImage*   getTableImage(void);
	                 void   returnTableImageToPool(TableImage* t);
	
	private:
	                        TableImageFactory(void);                                 
	                        TableImageFactory(const TableImageFactory&);           
	                        TableImageFactory& operator=(const TableImageFactory&);
	                       ~TableImageFactory(void);
	                  int   processId;
 std::vector<TableImage*>   tableImagePool;
	std::set<TableImage*>   allocatedTableImages;
};

#endif

