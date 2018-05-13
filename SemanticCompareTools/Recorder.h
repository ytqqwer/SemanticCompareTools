#pragma once

#include "xlnt/xlnt.hpp"

class Recorder
{
public:
	Recorder();
	~Recorder();

	void Init();

	void insertNewRecord(const std::vector<std::string>& word, const std::vector<std::string>& ids);
	
	void toNextWord();

	bool findRecord(const std::string&, const  std::string&, const  std::string&, const std::string&, const  std::string&, const  std::string&);
	
	void save();
	
	std::vector<std::string> getIDs();
	void updateIDs(const std::vector<std::string>&);

private:

	void insertWord(const std::vector<std::string>&);
	void insertID(const std::vector<std::string>&);

private:	
	xlnt::workbook wb;

	unsigned int curRow;
	unsigned int insertRow;

	unsigned int foundedRowOfID;

	std::vector<std::string> IDs;
};
