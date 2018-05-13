#pragma once

#include "xlnt/xlnt.hpp"

#include "common.h"

#include <map>  

class XlsxFile
{
public:
	XlsxFile(const std::string& fileName, const std::string& path);
	~XlsxFile();

	void setWordColumnName(const std::string& wordColumnName);
	void setDisplayColumnNames(const std::vector<std::string>& displayColumnNames);
	
	std::string getFileName();
	std::string getWordColumnName();
	std::vector<std::string> getColumnNames();
	std::vector<std::string> getDisplayColumnNames();

	unsigned int size();

	void fineWord(const std::string& word, std::vector<ListViewRow>& result);

private:
	unsigned int maxRow;

	std::string fileName;
	std::string filePath;

	std::string wordColumnName;//����������е�����

	xlnt::workbook workbook;

	std::multimap<std::string , int> selectedWord;

	std::vector<std::string> columnNames;//ȫ������
	std::vector<std::string> displayColumnNames;//Ҫ��ʾ���е�����
	
};
