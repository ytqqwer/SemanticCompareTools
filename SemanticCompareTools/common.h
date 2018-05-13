#pragma once

#include <string>
#include <vector>
#include <map>

#define MAX_LOADSTRING 100

struct FileNameAndColumnNames {
	HWND groupBox1;
	HWND groupBox2; 
	HWND hPOSEdit;
	std::string fileName;
	std::vector<std::string> columnNames;
	std::vector<HWND> checkBoxHandles;
	std::vector<HWND> radioButtonHandles;
};

struct ListViewRow {
	std::string fileName;
	std::map<std::string,std::string> subItems;
};

struct FileNameAndPartOfSpeech {
	std::string fileName;
	std::string partOfSpeech;
};
