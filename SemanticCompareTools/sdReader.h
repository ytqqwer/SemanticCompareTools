#pragma once

#include "XlsxFile.h"

#include <map>

class sdReader
{
public:
	sdReader();
	~sdReader();

	void addXlsxFile(const std::string& fileName, const std::string& path);

	void clear();

	void findWord(const std::string& fileName,const std::string& word, std::vector<ListViewRow>& result);
	void findWord(const std::string& word, std::vector<ListViewRow>& result);

	void setSelectedColumnNamesAndWordColumnName();
public:
	void setWordColumnName(const std::string& fileName , const std::string& wordColumnName);
	void setDisplayColumnNames(const std::string& fileName, const std::vector<std::string>& displayColumnNames);
	
	std::string getWordColumnName(const std::string& fileName);
	std::vector<std::string> getFileNames();
	std::vector<std::string> getColumnNames(const std::string& fileName);
	std::vector<std::string> getDisplayColumnNames(const std::string& fileName);	

private:
	std::vector<std::string> fileNames;

	std::unordered_map<std::string,XlsxFile> loadedXlsxFile;
};
