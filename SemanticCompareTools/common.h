#pragma once

#include <string>
#include <vector>
#include <map>

#define MAX_LOADSTRING 100

//const std::wstring PART_OF_SPEECH_DAI = L"r";
//const std::wstring PART_OF_SPEECH_DONG = L"v";
//const std::wstring PART_OF_SPEECH_FU = L"d";
//const std::wstring PART_OF_SPEECH_JIE = L"p";
//const std::wstring PART_OF_SPEECH_LIAN = L"c";
//const std::wstring PART_OF_SPEECH_LIANG = L"q";
//const std::wstring PART_OF_SPEECH_MING = L"n";
//const std::wstring PART_OF_SPEECH_NI = L"o";
//const std::wstring PART_OF_SPEECH_SHU = L"m";
//const std::wstring PART_OF_SPEECH_TAN = L"e";
//const std::wstring PART_OF_SPEECH_WEI = L"未";
//const std::wstring PART_OF_SPEECH_XING = L"a";
//const std::wstring PART_OF_SPEECH_ZHU = L"u";
//const std::wstring PART_OF_SPEECH_ZHUI = L"缀";


const std::wstring PART_OF_SPEECH_DAI = L"r";
const std::wstring PART_OF_SPEECH_DONG = L"v";
const std::wstring PART_OF_SPEECH_FU = L"d";
const std::wstring PART_OF_SPEECH_JIE = L"p";
const std::wstring PART_OF_SPEECH_LIAN = L"c";

const std::wstring PART_OF_SPEECH_LIANG = L"q";
const std::wstring PART_OF_SPEECH_MING = L"n";
const std::wstring PART_OF_SPEECH_NI = L"o";
const std::wstring PART_OF_SPEECH_SHU = L"m";
const std::wstring PART_OF_SPEECH_TAN = L"e";

const std::wstring PART_OF_SPEECH_WEI = L"未";
const std::wstring PART_OF_SPEECH_XING = L"a";
const std::wstring PART_OF_SPEECH_ZHU = L"u";
const std::wstring PART_OF_SPEECH_ZHUI = L"缀";

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
