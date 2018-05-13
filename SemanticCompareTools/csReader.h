#pragma once

#include "xlnt/xlnt.hpp"

class csReader
{
public:
	csReader();
	~csReader();

	void clear();
	void reset();
	bool isExistingFile();

	void addXlsxFileName(const std::string& filename);
	void loadXlsxFile(const std::string& pattern, const std::string& partOfSpeech, const std::string& path);
	
	bool prevWord();
	bool toPrevIsomorphicOfCurWord();
	bool nextWord();
	bool toNextIsomorphicOfCurWord();

	bool findWord(const std::string& word);

public:
	bool setPartOfSpeech(const std::string&);

	void setColumnNames(const std::vector<std::string>& columnNames);
	void setWordColumnName(const std::string& columnName);

	std::pair<unsigned int, std::vector<unsigned int>> getRowsByIsomorphicIndex();

	std::string getValueInColumnByRow(unsigned int row,const std::string& columnName);
	
	unsigned int getIsomorphicNumberOfCurWord();
	
private:
	unsigned int calMaxRow(xlnt::range&);

	bool skipEmptyWorkbook();
	void changeWorkbook(unsigned int index = 0);

	bool nextWorkbook();
	bool prevWorkbook();

	void selectPreviousIsomorphicWordGroup(unsigned int row);
	void selectNextIsomorphicWordGroup(unsigned int row);

	void selectColumn(unsigned int workbookIndex);

private:
	bool existingFile;
	//bool noWord;

private:

	unsigned int curWorkbookIndex;
	unsigned int maxRow;
	
	unsigned int curRow;		
	unsigned int curRowRangeBegin;//当前词组范围
	unsigned int curRowRangeEnd;
	
	unsigned int curIsomorphicIndex;

	std::string curPartOfSpeech;
	std::string wordColumnName;
	std::string curWord;

private:
	std::vector<std::string> fileNames;
	std::vector<std::string> columnNames;
	std::unordered_map<std::string, std::vector<xlnt::workbook>> loadedWorkbook;	 //词类和对应的工作簿

	std::unordered_map<std::string, xlnt::cell_vector> selColumns;	//根据传入的列名，保存对应的列

	std::multimap<std::string , int> selWordColumn;
	
	std::vector<std::pair<unsigned int , std::vector<unsigned int>>> selRows;	//词语所在行 以及 对应同形词语所在的行
	//int isomorphicWordRowOfSelRows;
		
};
