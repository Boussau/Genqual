#include <iostream>
#include <dirent.h>
#include "IoManager.h"



IoManager::IoManager(void) {

	setFileName("");
	setFilePath("");
	setCurDirectory( findCurrentDirectory() );
	setFilePath( getCurDirectory() );
}

IoManager::IoManager(std::string s) {

	parsePathFileNames(s);
	setCurDirectory( findCurrentDirectory() );
	if ( getFilePath() == "" )
		setFilePath( getCurDirectory() );
}

IoManager::~IoManager(void) {

}

bool IoManager::parsePathFileNames(std::string s) {

	std::string delimiter = "/";

	if ( s.length() == 0)
		{
		/* the string that is supposed to hold the
		   path/file information is empty. */
		std::cerr << "No path or file name provided" << std::endl;
		return false;
		}
		
	/* Find the location of the last "/". This is where
	   we will divide the path/file string into two. */
	int location = s.find_last_of( delimiter );
	
	if ( location == -1 )
		{
		/* There is no path in this string. We 
		   must have only the file name, and the
		   file should be in our current directory.*/
		fileName = s;
		filePath = "";
		}
	else if ( location == (int)s.length() - 1 )
		{
		/* It looks like the last character is "/", which
		   means that no file name has been provided. */
		s.erase( location );
		fileName = "";
		filePath = s;
		std::cerr << "Only a path name was provided" << std::endl;
		return false;
		}
	else
		{
		/* We can divide the path into the path and the
		   file. */
		fileName = s.substr( location+1, s.length()-location-1 );
		s.erase( location );
		filePath = s;
		}

	return true;
}

#define	MAX_DIR_PATH	2048
std::string IoManager::findCurrentDirectory(void) {

	std::string delimiter = "/";

	char cwd[MAX_DIR_PATH+1];
	if ( !getcwd(cwd, MAX_DIR_PATH+1) )
		{
		std::cerr << "Problem finding the current director" << std::endl;
		return "";
		}
	std::string curdir = cwd;
	
	if ( curdir.at( curdir.length()-1 ) == delimiter[0] )
		curdir.erase( curdir.length()-1 );
	
	return curdir;
}

std::string IoManager::getFilePathName(void) {

	std::string delimiter = "/";

	return filePath + delimiter + fileName;
}

bool IoManager::isDirectoryPresent(const std::string mp) {

	/* attempt to open the directory */
	DIR *dir = opendir( mp.c_str() );
	if ( !dir )
		return false;
		
	/* close the directory */
	if ( closedir(dir) == -1 )
		std::cerr << "Problem closing directory" << std::endl;
		
	return true;
}

bool IoManager::isFilePresent(const std::string mp, const std::string mf) {

	/* open the directory */
	DIR *dir = opendir( mp.c_str() );
	if ( !dir )
		{
		std::cerr << "Could not find path to directory" << std::endl;
		return false;
		}

	/* read the directory's contents */
	struct dirent *dirEntry;
	bool foundFile = false;
	while ( (dirEntry = readdir(dir)) != NULL ) 
		{
		std::string temp = dirEntry->d_name;
		if ( temp == mf )
			foundFile = true;
		}

	/* close the directory */
	if ( closedir(dir) == -1 )
		{
		std::cerr << "Problem closing directory" << std::endl;
		return false;
		}

	return foundFile;
}

bool IoManager::listDirectoryContents(void) {

	/* open the directory */
	DIR *dir = opendir( filePath.c_str() );
	if ( !dir )
		{
		std::cerr << "Could not find path to directory" << std::endl;
		return false;
		}

	/* read the directory's contents */
	struct dirent *dirEntry;
	while ( (dirEntry = readdir(dir)) != NULL ) 
		{
		std::cout << dirEntry->d_name << std::endl;
		}

	/* close the directory */
	if ( closedir(dir) == -1 )
		{
		std::cerr << "Problem closing directory" << std::endl;
		return false;
		}

	return true;	
}

int IoManager::getNumFilesInDirectory(void) {

	/* open the directory */
	DIR *dir = opendir( filePath.c_str() );
	if ( !dir )
		{
		std::cerr << "Could not find path to directory" << std::endl;
		return false;
		}

	/* read the directory's contents */
	struct dirent *dirEntry;
	int numFiles = 0;
	while ( (dirEntry = readdir(dir)) != NULL ) 
		{
		if (dirEntry->d_name[0] != '.')
			numFiles++;
		}

	/* close the directory */
	if ( closedir(dir) == -1 )
		{
		std::cerr << "Problem closing directory" << std::endl;
		return false;
		}

	return numFiles;
}

std::string IoManager::getFileNumber(int n) {

	/* open the directory */
	DIR *dir = opendir( filePath.c_str() );
	if ( !dir )
		{
		std::cerr << "Could not find path to directory" << std::endl;
		return false;
		}

	/* read the directory's contents */
	struct dirent *dirEntry;
	int numFiles = 0;
	std::string theFile = "";
	while ( (dirEntry = readdir(dir)) != NULL ) 
		{
		if (dirEntry->d_name[0] != '.')
			{
			if (numFiles == n)
				{
				theFile = dirEntry->d_name;
				break;
				}
			numFiles++;
			}
		}

	/* close the directory */
	if ( closedir(dir) == -1 )
		{
		std::cerr << "Problem closing directory" << std::endl;
		return false;
		}

	return theFile;
}

bool IoManager::openFile(std::ifstream &strm) {
	
	std::string delimiter = "/";

	/* concactenate path and file name */
	std::string filePathName = filePath + delimiter + fileName;

	/* here we assume that the presence of the path/file has
	   been checked elsewhere */
	strm.open( filePathName.c_str(), std::ios::in );
	if ( !strm )
		return false;
	return true;
}

bool IoManager::openFile(std::ofstream &strm) {
	
	std::string delimiter = "/";

	/* concactenate path and file name */
	std::string filePathName = filePath + delimiter + fileName;

	/* here we assume that the presence of the path/file has
	   been checked elsewhere */
	strm.open( filePathName.c_str(), std::ios::out );
	if ( !strm )
		return false;
	return true;
}

void IoManager::closeFile(std::ifstream &strm) {

	strm.close();
}

bool IoManager::testDirectory(void) {

	return isDirectoryPresent(filePath);
}

bool IoManager::testFile(void) {

	return isFilePresent(filePath, fileName);
}







