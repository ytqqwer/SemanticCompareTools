#pragma once

#include <vector>
#include <string>

struct tagWNDCLASSEXW;

class WindowClassRegister
{
public:
	WindowClassRegister();
	~WindowClassRegister();

	void registerClass(tagWNDCLASSEXW *);

public:
	static WindowClassRegister* getInstance();

private:
	static WindowClassRegister wcr;

	std::vector<std::string> classNames;
};
