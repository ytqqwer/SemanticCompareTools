#include "stdafx.h"
#include "XlsxFile.h"

#include "common.h"

XlsxFile::XlsxFile(const std::string& name, const std::string& path)
{
	fileName = name;
	filePath = path;	
	workbook.load(path + name);

	//计算出行数，算上首行列名
	//不用length，因为可能会有问题
	xlnt::worksheet& ws = workbook.active_sheet();
	auto& rows = ws.rows(false);
	unsigned int i = 0;
	for (auto& row : rows) {
		//if (row[0].to_string() != u8"") {
		//	i++;
		//}
		i++;
	}
	maxRow = i;

	//记录所有列名
	auto row = rows[0];
	for (auto& name : row) {
		std::string str = name.to_string();
		columnNames.push_back(str);
	}

}

XlsxFile::~XlsxFile()
{
}

void XlsxFile::setWordColumnName(const std::string & name)
{
	selectedWord.clear();

	wordColumnName = name;

	xlnt::worksheet& ws = workbook.active_sheet();
	auto& columns = ws.columns(false);
	for (auto& column : columns) {
		std::string fitstStr = column[0].to_string();

		//使用xLnt读取xlsx文件，返回值均为utf-8编码
		//故str中实际存储的是utf-8编码的字符串
		if (fitstStr == wordColumnName) {
			int index = 0;
			for (auto& word : column) {
				selectedWord.insert(std::make_pair(word.to_string(),index) );
				index++;
			}
			return;
		}
	}
	
}

void XlsxFile::setDisplayColumnNames(const std::vector<std::string>& names)
{
	displayColumnNames = names;
}

std::string XlsxFile::getFileName()
{
	return fileName;
}

std::string XlsxFile::getWordColumnName()
{
	return wordColumnName;
}

std::vector<std::string> XlsxFile::getColumnNames()
{
	return columnNames;
}

std::vector<std::string> XlsxFile::getDisplayColumnNames()
{
	return displayColumnNames;
}

unsigned int XlsxFile::size()
{
	return maxRow;
}

void XlsxFile::fineWord(const std::string & word, std::vector<ListViewRow>& result)
{	
	xlnt::worksheet& ws = workbook.active_sheet();
	auto& rows = ws.rows(false);
	auto& firstRow = rows[0];

	std::pair<std::multimap<std::string, int>::iterator, std::multimap<std::string, int>::iterator> range = selectedWord.equal_range(word);

	for (std::multimap<std::string,int>::iterator it= range.first; it!= range.second ; ++it) 
	{
		auto& row = rows[(*it).second];
		ListViewRow lvr;
		lvr.fileName = fileName;

		unsigned int size = columnNames.size();
		for (unsigned int index = 0; index < size; index++)
		{
			lvr.subItems.insert(std::make_pair(firstRow[index].to_string(), row[index].to_string()));
		}
		result.push_back(lvr);
	}
	

}
