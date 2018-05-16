#include "stdafx.h"
#include "csReader.h"
#include <codecvt>

#include <regex>	//正则表达式
#include <set>

csReader::csReader()
{
	maxRow = curRow = curRowRangeBegin = curRowRangeEnd = 1;
	curWorkbookIndex = 0;
	existingFile = false;

	curIsomorphicIndex = 0;
}

csReader::~csReader()
{
}

void csReader::clear()
{
	existingFile = false;

	fileNames.clear();
	loadedWorkbook.clear();
}

void csReader::reset()
{
	selColumns.clear();
	selWordColumn.clear();
	selRows.clear();
	curRow = 1;		//排除了列名所占的一行
	curRowRangeBegin = 1;
	curRowRangeEnd = 1;

	curIsomorphicIndex = 0;
}

bool csReader::isExistingFile()
{
	return existingFile;
}

void csReader::addXlsxFileName(const std::string & filename)
{
	fileNames.push_back(filename);
}

void csReader::loadXlsxFile(const std::string & pattern, const std::string & partOfSpeech, const std::string& path)
{
	std::regex re(pattern);
	for (auto& fileName : fileNames) {
		bool ret = std::regex_match(fileName, re);
		if (ret)
		{
			existingFile = true;	//设定已经加载了文件

			std::string fullPath = path + fileName;

			auto& iter = loadedWorkbook.find(partOfSpeech);
			if (iter != loadedWorkbook.end()) 
			{
				xlnt::workbook workbook;
				workbook.load(fullPath);
				iter->second.push_back(workbook);
				return;
			}

			//没有找到，说明还未添加该词类，则重新创建
			xlnt::workbook workbook;
			workbook.load(fullPath);
			std::vector<xlnt::workbook> wbs;
			wbs.push_back(workbook);

			loadedWorkbook.insert(make_pair(partOfSpeech, wbs));

			break;//继续寻找下一个相同词类的文件
		}
	}
}

bool csReader::setPartOfSpeech(const std::string & str)
{
	curPartOfSpeech = str;

	curWorkbookIndex = 0;

	reset();

	//重新选择工作簿，并且跳过空表
	return skipEmptyWorkbook();
}

unsigned int csReader::calMaxRow(xlnt::range & rows)
{
	int i = 0;
	for (auto& word : rows) {
		if (word[0].to_string() != u8"") {
			i++;
		}
	}
	return i;
}

bool csReader::skipEmptyWorkbook()
{
	auto& it = loadedWorkbook.find(curPartOfSpeech);
	if (it != loadedWorkbook.end()) {
		unsigned int workbookCounts = it->second.size();//工作簿总数，即当前词类对应的文件总数

		while (curWorkbookIndex < workbookCounts) {
			xlnt::worksheet& curWorksheet = it->second[curWorkbookIndex].active_sheet();
			maxRow = calMaxRow(curWorksheet.rows());

			if (maxRow > 1)//如果最大行数比1大
			{
				changeWorkbook(curWorkbookIndex);

				return true;
			}
			else
			{
				curWorkbookIndex++;
			}
		}
	}

	return false;
}

bool csReader::toPrevIsomorphicOfCurWord()
{
	if (0 < curIsomorphicIndex &&
		curIsomorphicIndex + 1 < getIsomorphicNumberOfCurWord())
	{
		curIsomorphicIndex--;
		return true;
	}
	else
		return false;
}

bool csReader::toNextIsomorphicOfCurWord()
{
	if (curIsomorphicIndex + 1 < getIsomorphicNumberOfCurWord()) {
		curIsomorphicIndex++;
		return true;
	}
	else
		return false;
}

bool csReader::prevWord()
{
	if (0 < curRowRangeBegin - 1) {
		selectPreviousIsomorphicWordGroup(curRowRangeBegin - 1);
		return true;
	}
	else
		return prevWorkbook();// 切换到上一个有效工作簿
}

bool csReader::nextWord()
{
	if (curRowRangeEnd + 1 < maxRow) {
		selectNextIsomorphicWordGroup(curRowRangeEnd + 1);
		return true;
	}
	else 
	{		
		return nextWorkbook();// 切换到下一个有效工作簿
	}

}

bool csReader::prevWorkbook()
{
	auto& it = loadedWorkbook.find(curPartOfSpeech);
	if (it != loadedWorkbook.end()) {
		unsigned int workbookCounts = it->second.size();//工作簿总数，即当前词类对应的文件总数

		while (curWorkbookIndex > 0 ) {
			curWorkbookIndex--;

			xlnt::worksheet& curWorksheet = it->second[curWorkbookIndex].active_sheet();
			maxRow = calMaxRow(curWorksheet.rows());

			if (maxRow > 1)//如果最大行数比1大
			{
				changeWorkbook(curWorkbookIndex);
				return true;
			}
		}
	}

	return false;
}

bool csReader::nextWorkbook()
{
	auto& it = loadedWorkbook.find(curPartOfSpeech);
	if (it != loadedWorkbook.end()) {
		unsigned int workbookCounts = it->second.size();//工作簿总数，即当前词类对应的文件总数

		while (curWorkbookIndex + 1 < workbookCounts) {	//防止索引越界
			curWorkbookIndex++;

			xlnt::worksheet& curWorksheet = it->second[curWorkbookIndex].active_sheet();
			maxRow = calMaxRow(curWorksheet.rows());

			if (maxRow > 1)//如果最大行数比1大
			{
				changeWorkbook(curWorkbookIndex);

				return true;
			}

		}
	}

	return false;

}

void csReader::changeWorkbook(unsigned int index)
{
	reset();
	selectColumn(index);//重新选择列

	selectNextIsomorphicWordGroup(1);//重新选择词组
}

void csReader::selectPreviousIsomorphicWordGroup(unsigned int row)
{
	selRows.clear();

	curRowRangeBegin = row;
	curRowRangeEnd = row;

	//获得词语的范围与同形数量
	std::set<std::string> setOfIsomorphic;
	curWord = getValueInColumnByRow(row, u8"gkb_词语");
	bool isOver = false;
	while (!isOver)
	{
		std::string& word = getValueInColumnByRow(row, u8"gkb_词语");
		if (curWord == word && word != u8"" && curWord != u8"")
		{
			setOfIsomorphic.insert(getValueInColumnByRow(row, u8"gkb_同形"));
			row--;
		}
		else
		{
			isOver = true;
		}
	}

	curRowRangeBegin = row + 1;

	//寻找同形所在行
	unsigned int row_of_isomorphic = maxRow;			//某一同形对应的词语所在行数，不一定是首个对应词语
	std::vector<unsigned int> matchedRows;	//匹配上的行
	for (auto& isomorphic : setOfIsomorphic) {
		for (row = curRowRangeBegin; row <= curRowRangeEnd; row++)
		{
			if (isomorphic == getValueInColumnByRow(row, u8"gkb_同形")) {
				row_of_isomorphic = row;
				matchedRows.push_back(row);
			}
		}
		selRows.push_back(make_pair(row_of_isomorphic, matchedRows));
		matchedRows.clear();
	}

	curIsomorphicIndex = 0;
	curRow = row;
}

//搜索词语同形的对应关系
void csReader::selectNextIsomorphicWordGroup(unsigned int row)
{
	selRows.clear();

	curRowRangeBegin = row;
	curRowRangeEnd = row;

	std::set<std::string> isomorphicSet;

	//获得词语的范围，同形的数量
	curWord = getValueInColumnByRow(row, u8"gkb_词语");
	bool isOver = false;
	while (!isOver) {
		std::string& word = getValueInColumnByRow(row, u8"gkb_词语");
		if (curWord == word && word != u8"" && curWord != u8"")
		{
			isomorphicSet.insert(getValueInColumnByRow(row, u8"gkb_同形"));
			row++;
		}
		else
		{
			isOver = true;
		}
	}
	curRowRangeEnd = row - 1;

	//寻找同形所在行
	unsigned int row_of_isomorphic = 1;			//某一同形对应的词语所在行，不一定是首个对应词语
	std::vector<unsigned int> matchedRows;		//匹配上的行
	for (auto& isomorphic : isomorphicSet) {

		for (row = curRowRangeBegin; row <= curRowRangeEnd; row++)
		{
			if (isomorphic == getValueInColumnByRow(row, u8"gkb_同形")) {
				row_of_isomorphic = row;
				matchedRows.push_back(row);
			}
		}

		selRows.push_back(make_pair(row_of_isomorphic, matchedRows));
		matchedRows.clear();
	}

	curIsomorphicIndex = 0;
	curRow = row;
}

void csReader::selectColumn(unsigned int workbookIndex)
{
	for (auto& columnName : columnNames) {

		auto& it = loadedWorkbook.find(curPartOfSpeech);
		if (it != loadedWorkbook.end()) 
		{		
			xlnt::worksheet& curWorksheet = it->second[workbookIndex].active_sheet();

			auto& columns = curWorksheet.columns(false);
			for (auto& column : columns) 
			{
				//使用xLnt读取xlsx文件，返回值均为utf-8编码
				//故str中实际存储的是utf-8编码的字符串
				if (columnName == column[0].to_string()) 
				{
					selColumns.insert(make_pair(columnName, column));

					//对词语所在列进行额外存储
					if (columnName == wordColumnName) {					

						int index = 0;
						for (auto& word : column) {
							selWordColumn.insert(std::make_pair(word.to_string(), index));
							index++;
						}

					}
					break;
				}
			}
		}
	}
	
}

std::pair<unsigned int, std::vector<unsigned int>> csReader::getRowsByIsomorphicIndex()
{
	if (selRows.size()) {
		return selRows[curIsomorphicIndex];	
	}

	return std::pair<unsigned int, std::vector<unsigned int>>();

}

std::string csReader::getValueInColumnByRow(unsigned int row, const std::string & columnName)
{
	auto& it = selColumns.find(columnName);
	if (it != selColumns.end()) {
		return it->second[row].to_string();
	}

	return std::string("none");
}

void csReader::setWordColumnName(const std::string & columnName)
{
	wordColumnName = columnName;
}

void csReader::setColumnNames(const std::vector<std::string>& names)
{
	columnNames = names;
}

unsigned int csReader::getIsomorphicNumberOfCurWord()
{
	return selRows.size();
}

bool csReader::findWord(const std::string & word)
{
	unsigned int preCurRow = curRow;
	unsigned int preWorkbookIndex = curWorkbookIndex;

	auto& it = loadedWorkbook.find(curPartOfSpeech);
	if (it != loadedWorkbook.end()) 
	{
		unsigned int workbookCounts = it->second.size();//工作簿总数，即当前词类对应的文件总数

		for (curWorkbookIndex = 0; curWorkbookIndex < workbookCounts; ++curWorkbookIndex)
		{
			changeWorkbook(curWorkbookIndex);
			
			auto& it = selWordColumn.find(word);
			if (it != selWordColumn.end()) {
				selectNextIsomorphicWordGroup(it->second);
				
				return true;
			}		
		}	
	}

	///////////////////////////////////////////////////
	//复原
	curWorkbookIndex = preWorkbookIndex;
	curRow = preCurRow;

	changeWorkbook(curWorkbookIndex);
	selectNextIsomorphicWordGroup(curRow);

	return false;
}
