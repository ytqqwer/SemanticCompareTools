#include "stdafx.h"
#include "SelectFunctionWindow.h"

#include "SenseAlignmentWindow.h"
#include "CheckSimilarityWindow.h"
#include "SearchDictionaryWindow.h"

#include "WindowClassRegister.h"
#include "xWindowModule.h"

extern xWindowModule winModule;

SelectFunctionWindow::SelectFunctionWindow(HINSTANCE instance) : xWindow(instance)
{
	wcscpy_s(className, L"SelectFunctionWindow");
	wcscpy_s(title, L"ѡ��");
}

SelectFunctionWindow::~SelectFunctionWindow()
{
}

void SelectFunctionWindow::registerClass()
{
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = xWindowModule::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = className;
	wcex.hIconSm = NULL;

	WindowClassRegister::getInstance()->registerClass(&wcex);
}

void SelectFunctionWindow::initWindow()
{
	winModule.AddXWindowObject(nullptr,this);

	hWnd = CreateWindowW(className, title, WS_CAPTION,
		CW_USEDEFAULT, 0, 150, 170, nullptr, nullptr, hInstance, nullptr);

	// �����꣬�ؼ����Դ���Ϊ��׼��  
	// ����ʱ��������  
	int yLoc = 0;

	//��ѡ��ť  
	yLoc += 10;
	senseAlignmentHandle = CreateWindow(L"Button", L"����Ա�", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
		12, yLoc, 120, 20, hWnd, (HMENU)ID_RADBTN_SENSEALI, hInstance, NULL);

	yLoc += 25;
	checkSimilarityHandle = CreateWindow(L"Button", L"������ƶ�", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		12, yLoc, 120, 20, hWnd, (HMENU)ID_RADBTN_CHECKSIM, hInstance, NULL);

	yLoc += 25;
	searchDictionaryHandle = CreateWindow(L"Button", L"�ʵ��ѯ", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
		12, yLoc, 120, 20, hWnd, (HMENU)ID_RADBTN_SEARCHDIC, hInstance, NULL);

	yLoc += 30;
	// ����һ��ȷ����ť  
	confirmButtonHandle = CreateWindow(L"Button", L"ѡ��", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		30, yLoc, 80, 27, hWnd, (HMENU)ID_BUTTON_OK, hInstance, NULL);

	//winModule.AddXObject(hWnd,this);

	HFONT hFont = CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, 0, 
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, 
		L"΢���ź�");//��������
	SendMessage(senseAlignmentHandle, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(checkSimilarityHandle, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(searchDictionaryHandle, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(confirmButtonHandle, WM_SETFONT, (WPARAM)hFont, TRUE);
}

LRESULT SelectFunctionWindow::processMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// �����˵�ѡ��: 
		switch (wmId)
		{
		case ID_BUTTON_OK: {
			if (SendMessage(senseAlignmentHandle, BM_GETCHECK, 0, 0) == TRUE)//�Ƿ���� 
			{
				senseAlignmentWindow* saw = new senseAlignmentWindow(hInstance);
				winModule.changeWindow(saw);
			}
			else if (SendMessage(checkSimilarityHandle, BM_GETCHECK, 0, 0) == TRUE)
			{
				CheckSimilarityWindow* ssw = new CheckSimilarityWindow(hInstance);
				winModule.changeWindow(ssw);
			}
			else if (SendMessage(searchDictionaryHandle, BM_GETCHECK, 0, 0) == TRUE)
			{
				SearchDictionaryWindow* sdw = new SearchDictionaryWindow(hInstance);
				winModule.changeWindow(sdw);
			}
		}	break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_DESTROY: {
		xWindowModule::postQuitMessage();
	}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

BOOL SelectFunctionWindow::activeControls(bool)
{
	return 0;
}
