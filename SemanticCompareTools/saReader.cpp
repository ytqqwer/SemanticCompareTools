#include "stdafx.h"
#include "saReader.h"
#include <codecvt>

#include <regex>	//正则表达式

saReader::saReader()
{
	maxRow = curRow  = 1;
	curWorkbookIndex = 0;
	existingFile = false;
}

saReader::~saReader()
{
}

void saReader::clear()
{
	existingFile = false;

	fileNames.clear();
	loadedWorkbook.clear();
}

void saReader::reset()
{
	selColumns.clear();
	selWordColumn.clear();

	curRow = 1;		//排除了列名所占的一行

}

bool saReader::isExistingFile()
{
	return existingFile;
}

void saReader::addXlsxFileName(const std::string & filename)
{
	fileNames.push_back(filename);
}

void saReader::loadXlsxFile(const std::string & pattern, const std::string & partOfSpeech, const std::string & path)
{
	std::regex re(pattern);
	for (auto& fileName : fileNames) {
		bool ret = std::regex_match(fileName, re);
		if (ret)
		{
			existingFile = true;	//设定已经加载了文件

			std::string& fullPath = path + fileName;

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

bool saReader::prevWord()
{
	if (0 < curRow - 1) {
		--curRow;
		return true;
	}
	else
		return prevWorkbook();// 切换到上一个有效工作簿
}

bool saReader::nextWord()
{
	if (curRow + 1 < maxRow) {
		++curRow;
		return true;
	}
	else
	{
		return nextWorkbook();// 切换到下一个有效工作簿
	}
}

bool saReader::setPartOfSpeech(const std::string & str)
{
	curPartOfSpeech = str;
	curWorkbookIndex = 0;

	reset();

	//重新选择工作簿，并且跳过空表
	return skipEmptyWorkbook();
}

void saReader::setColumnNames(const std::vector<std::string>& names)
{
	columnNames = names;
}

void saReader::setWordColumnName(const std::string & columnName)
{
	wordColumnName = columnName;
}

int saReader::getCurRowIndex()
{
	return curRow;
}

std::string saReader::getValueInColumnByRow(unsigned int row, const std::string & columnName)
{
	auto& it = selColumns.find(columnName);
	if (it != selColumns.end()) {
		return it->second[row].to_string();
	}

	return std::string("none");
}

unsigned int saReader::calMaxRow(xlnt::range & rows)
{
	int i = 0;
	for (auto& row : rows) 
	{
		if (row[0].to_string() != u8"") 
		{
			i++;
		}
	}
	return i;
}

bool saReader::skipEmptyWorkbook()
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
				++curWorkbookIndex;
			}
		}
	}

	return false;
}

void saReader::changeWorkbook(unsigned int index)
{
	reset();
	selectColumn(index);//重新选择列

	curRow = 0;
	nextWord();//重新选择词
}

bool saReader::nextWorkbook()
{
	auto& it = loadedWorkbook.find(curPartOfSpeech);
	if (it != loadedWorkbook.end()) {
		unsigned int workbookCounts = it->second.size();//工作簿总数，即当前词类对应的文件总数

		while (curWorkbookIndex + 1 < workbookCounts) {	//防止索引越界
			++curWorkbookIndex;

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

bool saReader::prevWorkbook()
{
	auto& it = loadedWorkbook.find(curPartOfSpeech);
	if (it != loadedWorkbook.end()) {
		unsigned int workbookCounts = it->second.size();//工作簿总数，即当前词类对应的文件总数

		while (curWorkbookIndex > 0) {
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

void saReader::selectColumn(unsigned int workbookIndex)
{
	for (auto& columnName : columnNames) {

		auto& it = loadedWorkbook.find(curPartOfSpeech);
		if (it != loadedWorkbook.end())
		{
			xlnt::worksheet& curWorksheet = it->second[workbookIndex].active_sheet();

			auto& columns = curWorksheet.columns(false);
			for (auto& column : columns)
			{
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

bool saReader::findWord(const std::string & word)
{
	unsigned int preCurRow = curRow;
	unsigned int preWorkbookIndex = curWorkbookIndex;

	bool recover = false;

	auto& it = loadedWorkbook.find(curPartOfSpeech);
	if (it != loadedWorkbook.end())
	{
		unsigned int workbookCounts = it->second.size();//工作簿总数，即当前词类对应的文件总数

		recover = true;

		for (curWorkbookIndex = 0; curWorkbookIndex < workbookCounts; ++curWorkbookIndex)
		{
			changeWorkbook(curWorkbookIndex);

			auto& it = selWordColumn.find(word);
			if (it != selWordColumn.end()) {
				curRow = it->second;
				return true;
			}
		}
	}
	else {
		recover = false;
	}

	///////////////////////////////////////////////////
	//复原
	if (recover == true) {
		curWorkbookIndex = preWorkbookIndex;
		curRow = preCurRow;

		changeWorkbook(curWorkbookIndex);
	}


	return false;
}
