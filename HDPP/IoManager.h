#ifndef IoManager_H
#define IoManager_H

#include <string>
#include <fstream>

class IoManager {

	public:
                            IoManager(void);
                            IoManager(std::string s);
						   ~IoManager(void);
			  std::string   getCurDirectory(void) { return curDirectory; }
			  std::string   getFileName(void) { return fileName; }
			  std::string   getFilePath(void) { return filePath; }
			  std::string   getFilePathName(void);
                     void   setCurDirectory(std::string s) { curDirectory = s; }
                     void   setFileName(std::string s) { fileName = s; }
                     void   setFilePath(std::string s) { filePath = s; }
                     void   closeFile(std::ifstream &strm);
                     bool   openFile(std::ifstream &strm);
                     bool   openFile(std::ofstream &strm);
                     bool   testDirectory(void);
                     bool   testFile(void);
                     bool   listDirectoryContents(void);
                     bool   parsePathFileNames(std::string s);
					  int   getNumFilesInDirectory(void);
			  std::string   getFileNumber(int n);

	private:
			  std::string   curDirectory;	
			  std::string   fileName;
			  std::string   filePath;
			  std::string   findCurrentDirectory(void);
                     bool   isDirectoryPresent(const std::string mp);
                     bool   isFilePresent(const std::string mp, const std::string mf);
};

#endif