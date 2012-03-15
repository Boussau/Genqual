#ifndef FranchiseHq_H
#define FranchiseHq_H

#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "MenuItem.h"

class Franchise;
class MbRandom;
class MenuItemInfo;
class Model;
class ParmTree;
class Settings;
class TableInfo;
class TableImage;

class FranchiseHq {

	public:
                                       FranchiseHq(Model* mp, Franchise* fp, Settings* sp, std::vector<std::string>& tn, std::vector<TableImage*>& tableImages);
                                       FranchiseHq(Model* mp, Franchise* fp, Settings *sp, std::vector<std::string>& tn, std::vector<MenuItemInfo*>& mi, std::vector<TableInfo*>& ti, std::vector<double>& si);
						              ~FranchiseHq(void);
				           MenuItem*   addMenuItem(void);
				           MenuItem*   addMenuItem(long int initSeed);
				                void   addTableImage(int rIdx, int tIdx, MenuItem* m);
					            void   checkAuxiliaryMenuItems(void);
					            void   checkConsistency(void);
					            void   clearTableImages(void);
				           MenuItem*   findMenuItemIndexed(int idx);
				           MenuItem*   findMenuItemWithTable(int rIdx, int tIdx);
				                 int   getIndexOfMenuItemWithTable(int rIdx, int tIdx);
			                     int   getNumTablesAtMenuItems(void);
					            void   getTableMenuAssociation(std::map<TableImage*,MenuItem*>& kv);
					             int   getNumOccuppiedMenuItems(void);
					             int   getNumUnoccuppiedMenuItems(void);
					            void   getMenuItemInfo(std::map<int,int>& mii);
					            int*   getMenuItemInfo(int& n);
  std::set<MenuItem*,comp_menuItem>&   getMenuItems(void) { return menuItems; }
					            int*   getTableImageInfo(int& n);
				             double*   getStickWeights(int& n);
				           MenuItem*   pickMenuItemAtRandom(MbRandom* rp);
			    	       MenuItem*   pickMenuItemFromStickWeights(MbRandom* rp);
				                void   print(void);
				                void   printInfo(void);
					            void   removeAllTableImages(void);
					            void   removeMenuItem(MenuItem* m);
					            void   resetTableImages(std::vector<TableImage*>& tableImages);
					            void   sampleStates(int n);
					            void   setStickLengths(void);
					            void   synchronizeSeeds(void);
					            void   updateParameters(void);

	private:
								bool   acceptOrReject(double lnX);
	                            void   closeFile(void);
	                             int   getNextMenuItemIndex(void);
					            void   getStringWithAllParameters(std::string& s);
	                            void   initializeFranchiseHq(std::vector<TableImage*>& tableImages);
					            void   initializeFranchiseHq(std::vector<MenuItemInfo*>& mi, std::vector<TableInfo*>& ti, std::vector<double>& si);
								void   openFile(void);
					            void   setTableMenuAssignments(void);
                                 int   processId;
					             int   numProcesses;
			              Franchise*   franchisePtr;
				              Model*   modelPtr;
	                       MbRandom*   ranPtr;
				           Settings*   settingsPtr;
				              double   menuAlpha;
					             int   numAuxiliaryMenuItems;
				                 int   parmType;
								 int   parmTreatment;
					             int   numRestaurants;
   std::set<MenuItem*,comp_menuItem>   menuItems;
            std::vector<std::string>   taxonNames;
					   std::set<int>   availableMenuItemIndices;
		               std::ofstream   hqStrm;
};

#endif