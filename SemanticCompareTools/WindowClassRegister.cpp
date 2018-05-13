#include "stdafx.h"
#include "WindowClassRegister.h"

#include <windows.h>

#include "util.h"

WindowClassRegister WindowClassRegister::wcr;

WindowClassRegister* WindowClassRegister::getInstance()
{
	return &wcr;
}

WindowClassRegister::WindowClassRegister()
{
}

WindowClassRegister::~WindowClassRegister()
{
}

void WindowClassRegister::registerClass(tagWNDCLASSEXW * wcex)
{
	char cstr[100];
	TcharToChar(wcex->lpszClassName, cstr);
	std::string className(cstr);

	for (auto& name : classNames) {
		if (name == className) 
		{
			return;
		}
	}
	classNames.push_back(className);
	RegisterClassExW(wcex);
}
