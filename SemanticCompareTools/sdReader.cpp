#include "stdafx.h"
#include "sdReader.h"

#include "IniFile.h"

sdReader::sdReader()
{
}

sdReader::~sdReader()
{
}

void sdReader::addXlsxFile(const std::string & fileName, const std::string& filePath)
{
	fileNames.push_back(fileName);

	XlsxFile xf(fileName, filePath);

	loadedXlsxFile.insert(std::make_pair(fileName, xf));
}

void sdReader::clear()
{
	fileNames.clear();
	loadedXlsxFile.clear();
}

std::vector<std::string> sdReader::getFileNames()
{
	return fileNames;
}

void sdReader::findWord(const std::string& fileName, const std::string & word, std::vector<ListViewRow>& result)
{	
	auto it = loadedXlsxFile.find(fileName);
	if (it != loadedXlsxFile.end()) 
	{
		(*it).second.fineWord(word, result);
	}	
}

void sdReader::findWord(const std::string & word, std::vector<ListViewRow>& result)
{
	for (auto& xf : loadedXlsxFile) 
		xf.second.fineWord(word, result);
}

void sdReader::setWordColumnName(const std::string & fileName, const std::string & columnName)
{
	auto it = loadedXlsxFile.find(fileName);
	if (it != loadedXlsxFile.end()) 
	{
		(*it).second.setWordColumnName(columnName);
	}

}

void sdReader::setDisplayColumnNames(const std::string & fileName, const std::vector<std::string>& displayColumnNames)
{
	auto it = loadedXlsxFile.find(fileName);
	if (it != loadedXlsxFile.end()) {
		(*it).second.setDisplayColumnNames(displayColumnNames);
	}

}

std::string sdReader::getWordColumnName(const std::string & fileName)
{
	auto it = loadedXlsxFile.find(fileName);
	if (it != loadedXlsxFile.end()) {
		return (*it).second.getWordColumnName();
	}

	return std::string();
}

std::vector<std::string> sdReader::getColumnNames(const std::string & fileName)
{
	auto it = loadedXlsxFile.find(fileName);
	if (it != loadedXlsxFile.end()) {
		return (*it).second.getColumnNames();
	}

	return std::vector<std::string>();
}

std::vector<std::string> sdReader::getDisplayColumnNames(const std::string & fileName)
{
	auto it = loadedXlsxFile.find(fileName);
	if (it != loadedXlsxFile.end()) {
		return (*it).second.getDisplayColumnNames();
	}

	return std::vector<std::string>();
}

void sdReader::setSelectedColumnNamesAndWordColumnName()
{
	for (auto& fileName : fileNames)
	{
		TCHAR tchar[1000];
		std::vector<std::wstring> columnNamesW;

		//SelectedColumnNames
		IniFile::getInstance()->GetProfileStringWithPath(STW_U8(fileName).c_str(), L"SelectedColumnNames", tchar);
		SplitWString(tchar, columnNamesW, L",");
		std::vector<std::string> columnNames;
		for (auto & wstr : columnNamesW) {
			if (wstr.length() != 0) {
				columnNames.push_back(WTS_U8(wstr));
			}
		}
		setDisplayColumnNames(fileName, columnNames);

		//WordColumnName
		IniFile::getInstance()->GetProfileStringWithPath(STW_U8(fileName).c_str(), L"WordColumnName", tchar);
		std::wstring wordColumnName = tchar;
		setWordColumnName(fileName, WTS_U8(wordColumnName));
	}

}