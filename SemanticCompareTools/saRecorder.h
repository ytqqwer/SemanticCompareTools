#pragma once

#include "xlnt/xlnt.hpp"

class saRecorder
{
public:
	saRecorder();
	~saRecorder();

	void Init();

	void save();

	void insertNewRecord(const std::vector<std::string>&);
	
	bool findRecord(const std::vector<std::string>&);
		
	void updateMappingRelation(const std::string&);

	std::string getNewMappingRelation();

private:	
	xlnt::workbook wb;

	unsigned int curRow;
	unsigned int insertRow;

	std::multimap<std::string, int> selWords;
};
