//SemanticCompareTools.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "SemanticCompareTools.h"

#include "xWindowModule.h"
#include "SelectFunctionWindow.h"

xWindowModule winModule;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	winModule.initInstance(hInstance);

	SelectFunctionWindow* sfw = new SelectFunctionWindow(hInstance);
	winModule.changeWindow(sfw);

	winModule.end();

	return 0;
}
