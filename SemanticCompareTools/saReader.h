#pragma once

#include "xlnt/xlnt.hpp"

class saReader
{
public:
	saReader();
	~saReader();

	void clear();
	void reset();
	bool isExistingFile();

	void addXlsxFileName(const std::string& filename);
	void loadXlsxFile(const std::string& pattern, const std::string& partOfSpeech, const std::string& path);

	bool prevWord();
	bool nextWord();

	bool findWord(const std::string& word);

public:
	bool setPartOfSpeech(const std::string&);

	void setColumnNames(const std::vector<std::string>& names);
	void setWordColumnName(const std::string& columnName);
	
	int getCurRowIndex();
	std::string getValueInColumnByRow(unsigned int row, const std::string& columnName);

private:
	unsigned int calMaxRow(xlnt::range&);

	bool skipEmptyWorkbook();
	void changeWorkbook(unsigned int index);

	bool nextWorkbook();
	bool prevWorkbook();

	void selectColumn(unsigned int workbookIndex);

private:
	bool existingFile;

private:

	unsigned int curWorkbookIndex;
	unsigned int curRow;
	unsigned int maxRow;

	std::string curPartOfSpeech;
	std::string wordColumnName;
	std::string curWord;

private:
	std::vector<std::string> fileNames;
	std::vector<std::string> columnNames;
	std::unordered_map<std::string, std::vector<xlnt::workbook>> loadedWorkbook;	 //词类和对应的工作簿

	std::unordered_map<std::string, xlnt::cell_vector> selColumns;	//根据传入的列名，保存对应的列

	std::multimap<std::string, int> selWordColumn;

};
