#include "stdafx.h"
#include "WordDetailWindow.h"

#include "windowClassRegister.h"
#include "xWindowModule.h"

extern xWindowModule winModule;


WordDetailWindow::WordDetailWindow(HINSTANCE instance) : xWindow(instance)
{
	wcscpy_s(className, L"WordDetailWindow");
	wcscpy_s(title, L"详情");

}

WordDetailWindow::~WordDetailWindow()
{
}

void WordDetailWindow::registerClass()
{
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SEMANTICCOMPARETOOLS));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = NULL;
	wcex.lpfnWndProc = xWindowModule::WndProc;
	wcex.lpszClassName = className;
	wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

	WindowClassRegister::getInstance()->registerClass(&wcex);
}

// 创建适应文本大小的窗口，以及调整输出格式
void WordDetailWindow::initWindow()
{
	winModule.AddXWindowObject(nullptr, this);


	hWnd = CreateWindowW(
		className,	title,	WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE,
		CW_USEDEFAULT,	CW_USEDEFAULT,	400,400,
		nullptr,	nullptr,	hInstance,	nullptr);
	
	unsigned int row = 0;
	for (auto& data : datas)
	{
		HWND handle = CreateWindow(_T("static"), (LPWSTR)STW_U8(data.first).c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
			10, 10 + cyChar * row,		 90, 30,			hWnd, NULL, hInstance, NULL);
		SendMessage(handle, WM_SETFONT, (WPARAM)GetStockObject(17), 0);
		
		if (data.second.length() > 20) {
			std::wstring& wContent = STW_U8(data.second);

			auto counts = wContent.length() / 20;
			int i = 0;
			while (i <= counts) {
				std::wstring& wstr = wContent.substr( i*20 ,20);

				handle = CreateWindow(_T("static"), (LPWSTR)wstr.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
					110, 10 + cyChar * row, 260, 30, hWnd, NULL, hInstance, NULL);
				SendMessage(handle, WM_SETFONT, (WPARAM)GetStockObject(17), 0);

				++row;
				++i;
			}
		
		}
		else {
			std::wstring& wstr = STW_U8(data.second);
			handle = CreateWindow(_T("static"), (LPWSTR)wstr.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
				110, 10 + cyChar * row,		 260, 30,			hWnd, NULL, hInstance, NULL);
			SendMessage(handle, WM_SETFONT, (WPARAM)GetStockObject(17), 0);
		
			++row;
		}

	}
	
	//重设窗口大小
	MoveWindow(hWnd, 200, 200, 400 , 60 + cyChar * row, false);
}

BOOL WordDetailWindow::activeControls(bool)
{
	return 0;
}

LRESULT WordDetailWindow::processMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE: 
		{
			HDC hdc = GetDC(hWnd);
			GetTextMetrics(hdc, &tm);

			cxChar = tm.tmAveCharWidth;
			cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
			cyChar = tm.tmHeight + tm.tmExternalLeading;  

			ReleaseDC(hWnd, hdc);
			return 0;
		}
		break;
		//case WM_COMMAND:
		//{
		//	switch (LOWORD(wParam))// 分析wParam低位
		//	{
		//	case ID_BUTTON_CONFIRM:
		//	{
		//		closeWindow();
		//	}
		//	break;
		//	}
		//}
		//break;
		case WM_DESTROY: {		//关闭窗口时总是处理
			//xWindowModule::postQuitMessage();//此窗口无需处理

			return 0;
		}
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void WordDetailWindow::setData(std::vector<std::pair<std::string, std::string>>& datasInOrder)
{
	datas = datasInOrder;
}

void WordDetailWindow::pushData(std::pair<std::string, std::string>& pair)
{
	datas.push_back(pair);
}
