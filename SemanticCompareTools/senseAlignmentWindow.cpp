#include "stdafx.h"
#include "senseAlignmentWindow.h"

#include "windowClassRegister.h"
#include "xWindowModule.h"

extern xWindowModule winModule;

// 静态数据成员必须在类定义 外 进行初始化
WNDPROC senseAlignmentWindow::oldSearchEditProc = NULL;

senseAlignmentWindow::senseAlignmentWindow(HINSTANCE instance) : xWindow(instance)
{
	wcscpy_s(className, L"senseAlignmentWindow");
	wcscpy_s(title, L"词义对比");
}

senseAlignmentWindow::~senseAlignmentWindow()
{
}

void senseAlignmentWindow::registerClass()
{
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = xWindowModule::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SEMANTICCOMPARETOOLS));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = MAKEINTRESOURCEW(ID_MENU_SENSEALIGNMENT);
	wcex.lpszClassName = className;
	wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

	WindowClassRegister::getInstance()->registerClass(&wcex);
}

void senseAlignmentWindow::initWindow()
{
	winModule.AddXWindowObject(nullptr, this);

	hWnd = CreateWindowW(className, title, WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX,
		CW_USEDEFAULT, 0, 800, 450, nullptr, nullptr, hInstance, nullptr);

	//////////////////////////////////////////////////////////////////////
	//初始化搜索编辑框
	hSearchEdit = CreateWindow(_T("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT,
		70, 30, 200, 30, hWnd, (HMENU)ID_EDIT_SEARCH, hInstance, NULL);

	senseAlignmentWindow::oldSearchEditProc =
		(WNDPROC)SetWindowLongPtr(hSearchEdit, GWLP_WNDPROC, (LONG_PTR)senseAlignmentWindow::searchEditProc);

	//////////////////////////////////////////////////////////////////////
	//初始化搜索类别下拉列表
	hClassComboBox = CreateWindow(WC_COMBOBOX, _T(""), CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		310, 30, 100, 500, hWnd, (HMENU)ID_COMBOBOX_SEARCH, hInstance, NULL);

	//// load the combobox with item list. Send a CB_ADDSTRING message to load each item
	//TCHAR temp[100];

	//for (int index = 0; index < 12; index++) {
	//	LoadStringW(hInstance, ID_PART_OF_SPEECH_N + index, temp, MAX_LOADSTRING);
	//	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)temp);
	//}

	//// Send the CB_SETCURSEL message to display an initial item in the selection field  
	//SendMessage(hClassComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	//////////////////////////////////////////////////////////////////////
	//初始化搜索按钮
	hSearchButton = CreateWindow(_T("BUTTON"), _T("搜索"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		430, 30, 100, 30, hWnd, (HMENU)ID_BUTTON_SEARCH, hInstance, NULL);

	//////////////////////////////////////////////////////////////////////
	//初始化文本	
	CreateWindow(_T("static"), _T("词语"), WS_CHILD | WS_VISIBLE | SS_LEFT, 30, 30, 30, 30, hWnd,
		NULL, hInstance, NULL);
	CreateWindow(_T("static"), _T("词类"), WS_CHILD | WS_VISIBLE | SS_LEFT, 270, 30, 30, 30, hWnd,
		NULL, hInstance, NULL);

	CreateWindow(_T("static"), _T("词典1"), WS_CHILD | WS_VISIBLE | SS_LEFT, 30, 110, 40, 30, hWnd,
		NULL, hInstance, NULL);
	CreateWindow(_T("static"), _T("词典2"), WS_CHILD | WS_VISIBLE | SS_LEFT, 30, 210, 40, 30, hWnd,
		NULL, hInstance, NULL);

	hSimilarityStaticText = CreateWindow(_T("static"), _T("相似度"), WS_CHILD | WS_VISIBLE | SS_LEFT, 550, 10, 100, 30, hWnd,
		(HMENU)ID_TEXT_STATIC_SIMILARITY, hInstance, NULL);
	hSimilarityText = CreateWindow(_T("static"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 650, 10, 90, 30, hWnd,
		(HMENU)ID_TEXT_SIMILARITY, hInstance, NULL);

	hRelationshipStaticText = CreateWindow(_T("static"), _T("对应关系"), WS_CHILD | WS_VISIBLE | SS_LEFT, 550, 40, 100, 30, hWnd,
		(HMENU)ID_TEXT_STATIC_RELATIONSHIP, hInstance, NULL);
	hRelationshipText = CreateWindow(_T("static"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 650, 40, 90, 30, hWnd,
		(HMENU)ID_TEXT_RELATIONSHIP, hInstance, NULL);

	hNewRelationshipStaticText = CreateWindow(_T("static"), _T("新对应关系"), WS_CHILD | WS_VISIBLE | SS_LEFT, 550, 70, 100, 30, hWnd,
		(HMENU)ID_TEXT_STATIC_NEW_RELATIONSHIP, hInstance, NULL);
	hNewRelationshipText = CreateWindow(_T("static"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 650, 70, 90, 30, hWnd,
		(HMENU)ID_TEXT_NEW_RELATIONSHIP, hInstance, NULL);

	HFONT hFont = CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"微软雅黑");//创建字体
	SendMessage(hSearchButton, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(hSearchEdit, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(hNewRelationshipStaticText, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(hRelationshipStaticText, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(hSimilarityStaticText, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息

	SendMessage(hNewRelationshipText, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(hRelationshipText, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(hSimilarityText, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息

	//////////////////////////////////////////////////////////////////////
	 //初始化词典1的列表视图
	{
		hDictionaryListView_One = CreateWindow(WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_NOSORTHEADER,
			30, 140, 600, 60, hWnd, (HMENU)ID_LISTVIEW_DICTIONARY_ONE, hInstance, NULL);

		//WCHAR szText[256];     // Temporary buffer.
		//int iCol;
		//LVCOLUMN lvc;
		//// Initialize the LVCOLUMN structure.
		//// The mask specifies that the format, width, text,
		//// and subitem members of the structure are valid.
		//lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

		//// Add the columns.
		//for (iCol = 0; iCol < 4; iCol++)
		//{
		//	lvc.iSubItem = iCol;
		//	lvc.pszText = szText;
		//	lvc.fmt = LVCFMT_LEFT;		// Left-aligned column.

		//	if (iCol < 1)				// Width of column in pixels.
		//		lvc.cx = 60;
		//	else if (iCol < 2)
		//		lvc.cx = 60;
		//	else
		//		lvc.cx = 240;

		//	// Load the names of the column headings from the string resources.
		//	LoadString(hInst, ID_DICTIONARY_COLUMN_WORDS + iCol, szText, sizeof(szText) / sizeof(szText[0]));

		//	// Insert the columns into the list view.
		//	if (ListView_InsertColumn(hDictionaryListView_One, iCol, &lvc) == -1)
		//		return FALSE;
		//}

		//////////////////////////////////////////////////////////////////////
		//初始化词典2的列表视图
		hDictionaryListView_Two = CreateWindow(WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_NOSORTHEADER,
			30, 240, 600, 60, hWnd, (HMENU)ID_LISTVIEW_DICTIONARY_TWO, hInstance, NULL);

		//// Add the columns.
		//for (iCol = 0; iCol < 4; iCol++)
		//{
		//	lvc.iSubItem = iCol;
		//	lvc.pszText = szText;
		//	lvc.fmt = LVCFMT_LEFT;		// Left-aligned column.

		//	if (iCol < 1)				// Width of column in pixels.
		//		lvc.cx = 60;
		//	else if (iCol < 2)
		//		lvc.cx = 60;
		//	else
		//		lvc.cx = 240;

		//	// Load the names of the column headings from the string resources.
		//	LoadString(hInst, ID_DICTIONARY_COLUMN_WORDS + iCol, szText, sizeof(szText) / sizeof(szText[0]));

		//	// Insert the columns into the list view.
		//	if (ListView_InsertColumn(hDictionaryListView_Two, iCol, &lvc) == -1)
		//		return FALSE;
		//}

	}
	//////////////////////////////////////////////////////////////////////
	//初始化对应关系按钮
	hRelationEqualButton = CreateWindow(_T("BUTTON"), _T("相等"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		650, 120, 100, 30, hWnd, (HMENU)ID_BUTTON_EQUAL, hInstance, NULL);

	hRelationNotEqualButton = CreateWindow(_T("BUTTON"), _T("不相等"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		650, 170, 100, 30, hWnd, (HMENU)ID_BUTTON_NOT_EQUAL, hInstance, NULL);

	hRelationUnsureButton = CreateWindow(_T("BUTTON"), _T("不确定"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		650, 220, 100, 30, hWnd, (HMENU)ID_BUTTON_UNSURE, hInstance, NULL);

	hRelationBelongButton = CreateWindow(_T("BUTTON"), _T("属于"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		650, 270, 100, 30, hWnd, (HMENU)ID_BUTTON_BELONG, hInstance, NULL);

	//////////////////////////////////////////////////////////////////////
	//下一个按钮
	hDetailButton = CreateWindow(_T("BUTTON"), _T("详情"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		200, 330, 100, 30, hWnd, (HMENU)ID_BUTTON_DETAIL, hInstance, NULL);

	hNextWordButton = CreateWindow(_T("BUTTON"), _T("下一词语"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		350, 330, 100, 30, hWnd, (HMENU)ID_BUTTON_NEXT_WORD, hInstance, NULL);

}

BOOL senseAlignmentWindow::activeControls(bool boolean)
{




	return 0;
}

LRESULT senseAlignmentWindow::processMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
		{
			if (hWnd == hSearchEdit) {
				//Do your stuff
				MessageBox(hWnd, L"您点击了一个按钮。", L"提示", MB_OK);
				break;  //or return 0; if you don't want to pass it further to def proc
						//If not your key, skip to default:						
			}

		}

		}
		break;
	case WM_COMMAND:
	{
		// 分析wParam高位: 
		switch (HIWORD(wParam))
		{
		case CBN_SELCHANGE:
		{


		}
		break;
		default:
			//继续处理
			break;
		}

		// 分析wParam低位: 
		switch (LOWORD(wParam))
		{
		case ID_MENUITEM_OPEN:
		{

		}
		break;
		case ID_MENUITEM_EXIT: {
			closeWindow();
		}
							   break;
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

LRESULT CALLBACK senseAlignmentWindow::searchEditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
			//Do your stuff

			MessageBox(hWnd, L"您点击了一个按钮。", L"提示", MB_OK);

			break;  //or return 0; if you don't want to pass it further to def proc
					//If not your key, skip to default:
		}
	default:
		return CallWindowProc(senseAlignmentWindow::oldSearchEditProc, hWnd, msg, wParam, lParam);
	}
	return 0;
}
