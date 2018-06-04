#include "stdafx.h"
#include "csRecorder.h"
#include <codecvt>

#include <iostream>
#include <fstream>

csRecorder::csRecorder()
{
}

csRecorder::~csRecorder()
{
}

void csRecorder::Init()
{
	std::fstream _file;
	_file.open(u8"SimilarityRecord.xlsx",std::ios::in);
	if (!_file)
	{
		//没有被创建
		xlnt::worksheet& ws = wb.active_sheet();
	
		ws.cell("A1").value(u8"gkb_词语");
		ws.cell("B1").value(u8"gkb_词类");
		ws.cell("C1").value(u8"gkb_拼音");
		ws.cell("D1").value(u8"gkb_同形");
		ws.cell("E1").value(u8"gkb_释义");
		ws.cell("F1").value(u8"gkb_例句");
		ws.cell("G1").value(u8"ID");
		ws.cell("H1").value(u8"Check");

		wb.save(u8"SimilarityRecord.xlsx");

		//跳过列名所在的第一行，从第二行位置插入，下标从1开始
		insertRow = 2;
	}
	else
	{
		//已经存在
		wb.load(u8"SimilarityRecord.xlsx");

		//获取最大行数
		auto& ws = wb.active_sheet();
		auto highest_row = ws.highest_row();
		insertRow = highest_row;
		++insertRow;

		int index = 1;
		auto& rows = ws.rows();
		for (auto& row : rows) {		
			selWords.insert(make_pair(row[0].to_string(),index));
			++index;
		}

		return;
	}
}

void csRecorder::insertNewRecord(const std::vector<std::string>& wordInfo, const std::vector<std::string>& ids)
{
	insertWord(wordInfo);
	insertID(ids);

	selWords.insert(make_pair(wordInfo[0], insertRow));
	++insertRow;
}

void csRecorder::insertWord(const std::vector<std::string>& wordInfo)
{
	xlnt::worksheet& ws = wb.active_sheet();
	
	unsigned int i = 1;
	for (auto& str : wordInfo) 
	{
		xlnt::cell& cell = ws.cell(i, insertRow);
		cell.value(str);
		i++;
	}
}

void csRecorder::insertID(const std::vector<std::string>& IDs)
{
	xlnt::worksheet& ws = wb.active_sheet();
	xlnt::cell& cell = ws.cell(7, insertRow);

	std::string str;
	for (auto& id : IDs)
	{
		str = str + "," + id;
	}
	cell.value(str);	
}

void csRecorder::save()
{
	wb.save(u8"SimilarityRecord.xlsx");
}

bool csRecorder::findRecord(const std::string& word, const std::string& pos, const std::string& alphabetic,
	const std::string& isomorphic, const std::string& meanings, const std::string& example)
{
	auto& ws = wb.active_sheet();
	auto& rows = ws.rows(false);
	IDs.clear();

	auto& range = selWords.equal_range(word);

	for (auto& it = range.first; it != range.second; ++it) 
	{
		int index = it->second;

		auto& row = rows[index - 1];

		auto& tPos = row[1].to_string();
		auto& tAlphabetic = row[2].to_string();
		auto& tIsomorphic = row[3].to_string();
		auto& tMeanings = row[4].to_string();
		auto& tExample = row[5].to_string();
		
		//找到完全匹配项
		if (tPos == pos && tAlphabetic == alphabetic && tIsomorphic == isomorphic
			&& tMeanings == meanings && tExample == example)
		{
			//设置当前行
			curRow = index;

			// 取得ID
			auto& str = row[6].to_string();
			SplitString(str, IDs, ",");
			return true;
		}	
	}	

	//curRow = -1;	
	//for (auto& row: rows) {
	//	curRow ++;

	//	auto& tWord = row[0].to_string();
	//	
	//	if (tWord == word) {
	//		auto& tPos = row[1].to_string();
	//		auto& tAlphabetic = row[2].to_string();
	//		auto& tIsomorphic = row[3].to_string();
	//		auto& tMeanings = row[4].to_string();
	//		auto& tExample = row[5].to_string();
	//		//找到完全匹配项
	//		if ( tPos == pos  && tAlphabetic == alphabetic && tIsomorphic == isomorphic
	//			&& tMeanings == meanings && tExample == example) 
	//		{
	//			///////////////////////////
	//			// 取得ID
	//			auto& str = row[6].to_string();
	//			SplitString(str,  IDs, ",");
	//			return true;
	//		}
	//	}
	//	else {
	//		continue;
	//	}

	//}

	return false;
	
}

std::vector<std::string> csRecorder::getIDsAfterFind()
{
	return IDs;
}

void csRecorder::updateIDs(const std::vector<std::string>& IDs)
{
	auto& ws = wb.active_sheet();
	auto& rows = ws.rows(false);
	auto& row = rows[curRow - 1];

	auto& cell = row[6];

	std::string str;

	for (auto& id : IDs)
	{
		str = str + "," + id;
	}
	cell.value(str);

	xlnt::cell& tcell = ws.cell(8, curRow);
	tcell.value(u8"new");
}
