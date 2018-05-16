#include "stdafx.h"
#include "saRecorder.h"
#include <codecvt>

#include <iostream>
#include <fstream>

saRecorder::saRecorder()
{
}

saRecorder::~saRecorder()
{
}

void saRecorder::Init()
{
	std::fstream _file;
	_file.open(u8"record.xlsx",std::ios::in);
	if (!_file)
	{
		//没有被创建
		xlnt::worksheet& ws = wb.active_sheet();
	
		ws.cell("A1").value(u8"ID");
		ws.cell("B1").value(u8"词语");
		ws.cell("C1").value(u8"义项编码");
		ws.cell("D1").value(u8"拼音");
		ws.cell("E1").value(u8"词性");
		ws.cell("F1").value(u8"义项释义");
		ws.cell("G1").value(u8"示例");
		ws.cell("H1").value(u8"gkb_词语");
		ws.cell("I1").value(u8"gkb_词类");
		ws.cell("J1").value(u8"gkb_拼音");
		ws.cell("K1").value(u8"gkb_同形");
		ws.cell("L1").value(u8"gkb_释义");
		ws.cell("M1").value(u8"gkb_例句");
		ws.cell("N1").value(u8"映射关系");
		ws.cell("O1").value(u8"新映射关系");

		wb.save(u8"record.xlsx");

		//跳过列名所在的第一行，从第二行位置插入，下标从1开始
		insertRow = 2;
	}
	else
	{
		//已经存在
		wb.load(u8"record.xlsx");

		//获取最大行数
		auto& ws = wb.active_sheet();
		auto highest_row = ws.highest_row();
		insertRow = highest_row;
		++insertRow;

		int index = 1;
		auto& rows = ws.rows();
		for (auto& row : rows) {
			selWords.insert(make_pair(row[1].to_string(),index));
			++index;
		}

		return;
	}
}

void saRecorder::save()
{
	wb.save(u8"record.xlsx");
}

void saRecorder::insertNewRecord(const std::vector<std::string>& datas)
{
	xlnt::worksheet& ws = wb.active_sheet();
	
	int i = 1;
	for (auto& str : datas) 
	{
		xlnt::cell& cell = ws.cell(i, insertRow);
		cell.value(str);
		i++;
	}

	selWords.insert(make_pair(datas[1], insertRow));
	++insertRow;
}

bool saRecorder::findRecord(const std::vector<std::string>& datas)
{
	auto& ws = wb.active_sheet();
	auto& rows = ws.rows(false);

	auto& range = selWords.equal_range(datas[1]);//0为ID，1为词语

	for (auto& it = range.first; it != range.second; ++it)
	{
		int index = it->second;
		auto& row = rows[index - 1];

		if (
				datas[0] == row[0].to_string() &&	//ID
				datas[1] == row[1].to_string() &&	//词语
				datas[2] == row[2].to_string() &&	//义项编码
				datas[3] == row[3].to_string() &&	//拼音
				datas[4] == row[4].to_string() &&	//词性
				datas[5] == row[5].to_string() &&	//义项释义
				datas[6] == row[6].to_string() &&	//示例
				datas[7] == row[7].to_string() &&	//gkb_词语
				datas[8] == row[8].to_string() &&	//gkb_词类
				datas[9] == row[9].to_string() &&	//gkb_拼音
				datas[10] == row[10].to_string() &&	//gkb_同形
				datas[11] == row[11].to_string() &&	//gkb_释义
				datas[12] == row[12].to_string() 	//gkb_例句
				//&& datas[13] == row[13].to_string() 	//映射关系
			) {		
			//设置当前行
			curRow = index;
			return true;
		}
	}

	return false;
}

void saRecorder::updateMappingRelation(const std::string& relation)
{
	auto& ws = wb.active_sheet();
	auto& cell = ws.cell(15, curRow);

	//auto& rows = ws.rows(false);
	//auto& row = rows[curRow - 1];
	//auto& cell = row[14];//新映射关系

	cell.value(relation);
}

std::string saRecorder::getNewMappingRelation()
{
	auto& ws = wb.active_sheet();
	auto& cell = ws.cell(15, curRow);

	//auto& rows = ws.rows(false);
	//auto& row = rows[curRow - 1];
	//auto& cell = row[14];//新映射关系

	//return std::string();
	return cell.to_string();	
}
