#pragma once

#include "xlnt/xlnt.hpp"

class csRecorder
{
public:
	csRecorder();
	~csRecorder();

	void Init();

	void save();

	void insertNewRecord(const std::vector<std::string>& word, const std::vector<std::string>& ids);
	
	bool findRecord(const std::string&, const  std::string&, const  std::string&, const std::string&, const  std::string&, const  std::string&);
		
	std::vector<std::string> getIDsAfterFind();

	void updateIDs(const std::vector<std::string>&);

private:

	void insertWord(const std::vector<std::string>&);
	void insertID(const std::vector<std::string>&);

private:	
	xlnt::workbook wb;

	unsigned int curRow;
	unsigned int insertRow;

	std::multimap<std::string, int> selWords;

	std::vector<std::string> IDs;
};
