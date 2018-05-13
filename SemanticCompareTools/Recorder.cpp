#include "stdafx.h"
#include "Recorder.h"
#include <codecvt>

#include <iostream>
#include <fstream>

Recorder::Recorder()
{

}

Recorder::~Recorder()
{
}

void Recorder::Init()
{
	std::fstream _file;
	_file.open(u8"record.xlsx",std::ios::in);
	if (!_file)
	{
		//没有被创建
		xlnt::worksheet ws = wb.active_sheet();
	
		ws.cell("A1").value(u8"gkb_词语");
		ws.cell("B1").value(u8"gkb_词类");
		ws.cell("C1").value(u8"gkb_拼音");
		ws.cell("D1").value(u8"gkb_同形");
		ws.cell("E1").value(u8"gkb_释义");
		ws.cell("F1").value(u8"gkb_例句");
		ws.cell("G1").value(u8"ID");

		wb.save(u8"record.xlsx");

		insertRow = 2;//跳过列名所在的第一行
	}
	else
	{
		//已经存在
		wb.load(u8"record.xlsx");

		//获取最大行数
		auto& ws = wb.active_sheet();
		auto highest_row = ws.highest_row();
		insertRow = highest_row;
		insertRow++;

		return;
	}
}

void Recorder::insertNewRecord(const std::vector<std::string>& word, const std::vector<std::string>& ids)
{
	insertWord(word);
	insertID(ids);

	toNextWord();
}

void Recorder::insertWord(const std::vector<std::string>& word)
{
	xlnt::worksheet ws = wb.active_sheet();
	
	unsigned int i = 1;
	for (auto& str : word) 
	{
		xlnt::cell cell = ws.cell(i, insertRow);
		cell.value(str);
		i++;
	}

}

void Recorder::insertID(const std::vector<std::string>& IDs)
{
	xlnt::worksheet ws = wb.active_sheet();
	//xlnt::cell cell = ws.cell(u8"ID", insertRow);
	xlnt::cell cell = ws.cell(7, insertRow);
	auto r = cell.row();
	auto c = cell.column();
	std::string str;
	for (auto& id : IDs)
	{
		str = str + "," + id;
	}
	cell.value(str);	
}

void Recorder::toNextWord()
{
	insertRow++;
}

void Recorder::save()
{
	wb.save(u8"record.xlsx");
}

bool Recorder::findRecord(const std::string& word, const std::string& pos, const std::string& alphabetic,
	const std::string& isomorphic, const std::string& meanings, const std::string& example)
{
	auto ws = wb.active_sheet();
	auto rows = ws.rows(false);
	curRow = -1;

	IDs.clear();
	
	for (auto& row: rows) {
		curRow ++;

		auto& tWord = row[0].to_string();
		
		if (tWord == word) {
			auto& tPos = row[1].to_string();
			auto& tAlphabetic = row[2].to_string();
			auto& tIsomorphic = row[3].to_string();
			auto& tMeanings = row[4].to_string();
			auto& tExample = row[5].to_string();
			//找到完全匹配项
			if ( tPos == pos  && tAlphabetic == alphabetic && tIsomorphic == isomorphic
				&& tMeanings == meanings && tExample == example) 
			{
				///////////////////////////
				// 取得ID
				auto& str = row[6].to_string();
				SplitString(str,  IDs, ",");
				return true;
			}
		}
		else {
			continue;
		}

	}

	return false;
	
}

std::vector<std::string> Recorder::getIDs()
{
	return IDs;
}

void Recorder::updateIDs(const std::vector<std::string>& IDs)
{
	auto ws = wb.active_sheet();
	auto rows = ws.rows(false);
	auto row = rows[curRow];

	auto cell = row[6];

	std::string str;
	for (auto& id : IDs)
	{
		str = str + "," + id;
	}
	cell.value(str);
}
