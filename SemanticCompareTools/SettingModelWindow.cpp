#include "stdafx.h"
#include "SettingModelWindow.h"

#include "windowClassRegister.h"
#include "IniFile.h"
#include "xWindowModule.h"

#include "SearchDictionaryWindow.h"
#include "sdReader.h"

extern xWindowModule winModule;

SettingModelWindow::SettingModelWindow(HINSTANCE instance) : xWindow(instance),hParent(NULL)
{
	wcscpy_s(className, L"SettingModelWindow");
	wcscpy_s(title, L"设置");

}

SettingModelWindow::SettingModelWindow(HINSTANCE instance, HWND parent) : xWindow(instance),hParent(parent)
{
	wcscpy_s(className, L"SettingModelWindow");
	wcscpy_s(title, L"设置");

}

SettingModelWindow::~SettingModelWindow()
{
}

void SettingModelWindow::setParentWindowPtr(xWindow * ptr)
{
	parentWindowPtr = ptr;
}

void SettingModelWindow::registerClass()
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

void SettingModelWindow::initWindow()
{
	winModule.AddXWindowObject(nullptr, this);

	hWnd = CreateWindowW(className,title, WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME ,
		50,50,700, 650,hParent,NULL,	hInstance,	NULL);

	hSaveButton = CreateWindow(_T("BUTTON"), _T("保存"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		420, 550, 100, 30, hWnd, (HMENU)ID_BUTTON_SAVE, hInstance, NULL);

	hCancelButton = CreateWindow(_T("BUTTON"), _T("取消"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		550, 550, 100, 30, hWnd, (HMENU)ID_BUTTON_CANCEL, hInstance, NULL);

	//////////////////////////////////////////////////////////////////////

	hDicNameText = CreateWindow(_T("static"), _T("词典名"), WS_CHILD | WS_VISIBLE | SS_LEFT, 25, 560, 50, 30, hWnd,
		NULL, hInstance, NULL);

	//词典名编辑框
	hDicNameEdit = CreateWindow(_T("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT,
		80, 550, 180, 30, hWnd, (HMENU)ID_EDIT_DIC_NAME, hInstance, NULL);

	TCHAR temp[100];
	IniFile::getInstance()->GetProfileStringWithPath(L"", L"CustomDicName", temp);
	SetWindowText(hDicNameEdit, temp);
	
	HFONT hFont = CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"微软雅黑");//创建字体
	SendMessage(hDicNameEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hSaveButton, WM_SETFONT, (WPARAM)GetStockObject(17), 0);
	SendMessage(hCancelButton, WM_SETFONT, (WPARAM)GetStockObject(17), 0);
	SendMessage(hDicNameText, WM_SETFONT, (WPARAM)GetStockObject(17), 0);
	
	/////////////////////////////////////////////////////////////////////////
	//初始化文件名列表视图
	hFileNameListView = CreateWindowW(WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_NOSORTHEADER,
		25, 25, 200, 500, hWnd, (HMENU)ID_LISTVIEW_FILENAME, hInstance, NULL);

	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 0;
	lvc.pszText = TEXT("文件名");
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = 200;
	ListView_InsertColumn(hFileNameListView, 0, &lvc);

	//清空，重新加载
	FNACN.clear();

	//加载文件名
	LVITEM vitem;
	vitem.mask = LVIF_TEXT;
	vitem.iItem = 0;
	vitem.iSubItem = 0;

	SearchDictionaryWindow* ptr = (SearchDictionaryWindow* )parentWindowPtr;	
	auto fileNames = ptr->reader->getFileNames();
	for (auto name : fileNames) {
		FileNameAndColumnNames fncn;
		fncn.fileName = name;
		fncn.columnNames = ptr->reader->getColumnNames(name);
		FNACN.push_back(fncn);

		std::wstring wstr;
		wstr = stringToWstring(name);
		vitem.pszText = (LPWSTR)wstr.c_str();
		ListView_InsertItem(hFileNameListView, &vitem);
		vitem.iItem++;

	}

	/////////////////////////////////////////////////////////////////////////
	//对每一个文件名，创建一系列单选或多选框，使用GroupBox包装
	for (auto& fncn : FNACN) {

		int size = fncn.columnNames.size();
		int rowCount;
		if (size % 4) {
			rowCount = size / 4 + 1;
		}
		else {
			rowCount = size / 4;
		}

		//加载GroupBox
		HWND groupBox1 = CreateWindow(TEXT("Button"), TEXT("选择要显示的列"), WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
			250, 
			55, 
			410, 
			rowCount * 30 + 25,
			hWnd, NULL, hInstance, NULL);
		HWND groupBox2 = CreateWindow(TEXT("Button"), TEXT("选择词语所在的列"), WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
			250, 
			55 + rowCount * 30 + 30, 
			410, 
			rowCount * 30 + 25,
			hWnd, NULL, hInstance, NULL);

		SendMessage(groupBox1, WM_SETFONT, (WPARAM)GetStockObject(17), 0);
		SendMessage(groupBox2, WM_SETFONT, (WPARAM)GetStockObject(17), 0);
		fncn.groupBox1 = groupBox1;
		fncn.groupBox2 = groupBox2;

		//加载CheckBox
		for (int i = 0; i < size; i++) {

			auto name = fncn.columnNames[i];
			std::wstring wstr;
			wstr = stringToWstring(name);

			HWND checkBox = CreateWindow(TEXT("Button"), wstr.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
				10 + i % 4 * 100, 20 + i / 4 * 30, 90, 30,
				groupBox1, NULL, hInstance, NULL);
			SendMessage(checkBox, WM_SETFONT, (WPARAM)GetStockObject(17), 0);

			fncn.checkBoxHandles.push_back(checkBox);
		}

		//加载RadioButton
		for (int i = 0; i < size; i++) {
			auto name = fncn.columnNames[i];
			std::wstring wstr;
			wstr = stringToWstring(name);

			HWND radioButton = CreateWindow(TEXT("Button"), wstr.c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
				10 + i % 4 * 100, 20 + i / 4 * 30, 90, 30,
				groupBox2, NULL, hInstance, NULL);
			SendMessage(radioButton, WM_SETFONT, (WPARAM)GetStockObject(17), 0);

			fncn.radioButtonHandles.push_back(radioButton);
		}

		//词类名编辑框
		HWND hPOSEdit = CreateWindow(_T("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT,
			350, 15, 200, 30, hWnd, (HMENU)ID_EDIT_PART_OF_SPEECH, hInstance, NULL);
		SendMessage(hPOSEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
		fncn.hPOSEdit = hPOSEdit;
	}

	hPosText = CreateWindow(_T("static"), _T("自定义词类"), WS_CHILD | WS_VISIBLE | SS_LEFT, 260, 25, 70, 30, hWnd,
		NULL, hInstance, NULL);
	SendMessage(hPosText, WM_SETFONT, (WPARAM)GetStockObject(17), 0);

	/////////////////////////////////////////////////////////////////////////
	//默认只显示一组GroupBox
	for (auto& fncn : FNACN) {
		ShowWindow(fncn.groupBox1, SW_HIDE);
		ShowWindow(fncn.groupBox2, SW_HIDE);
		ShowWindow(fncn.hPOSEdit, SW_HIDE);
	}
	ShowWindow(FNACN[0].groupBox1, SW_SHOW);
	ShowWindow(FNACN[0].groupBox2, SW_SHOW);
	ShowWindow(FNACN[0].hPOSEdit, SW_SHOW);

	/////////////////////////////////////////////////////////////////////////
	// 按钮打勾，设置自定义词类名
	for (auto& fncn : FNACN)
	{
		TCHAR tchar[1000];
		std::vector<std::wstring> columnNames;

		std::wstring wFileName = STW_U8(fncn.fileName);

		//check box
		IniFile::getInstance()->GetProfileStringWithPath(wFileName.c_str(), L"SelectedColumnNames", tchar);
		SplitWString(tchar, columnNames, L",");

		for (auto& handle : fncn.checkBoxHandles) {
			GetWindowText(handle, tchar, 1000);
			std::wstring temp = tchar;

			for (auto& name : columnNames)
			{
				if (temp == name)
				{
					SendMessage(handle, BM_SETCHECK, BST_CHECKED, 0);
					break;
				}
			}
		}

		//radio button
		IniFile::getInstance()->GetProfileStringWithPath(wFileName.c_str(), L"WordColumnName", tchar);
		std::wstring wordColumnName = tchar;
		for (auto& handle : fncn.radioButtonHandles) {
			GetWindowText(handle, tchar, 1000);
			std::wstring temp = tchar;

			if (temp == wordColumnName)
			{
				SendMessage(handle, BM_SETCHECK, BST_CHECKED, 0);
				break;
			}
		}
		
		//part of speech
		IniFile::getInstance()->GetProfileStringWithPath(wFileName.c_str(), L"PartOfSpeech", temp);
		SetWindowText(fncn.hPOSEdit, temp);
		
	}


}

BOOL SettingModelWindow::activeControls(bool)
{
	return 0;
}

LRESULT SettingModelWindow::processMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_NOTIFY:
	{
		switch (wParam)
		{
		case ID_LISTVIEW_FILENAME:
		{
			LPNMITEMACTIVATE activeItem = (LPNMITEMACTIVATE)lParam; //得到NMITEMACTIVATE结构指针  
			if (activeItem->iItem >= 0)				//当点击空白区域时，iItem值为-1，防止越界
			{
				switch (activeItem->hdr.code) {		//判断通知码  
				case NM_CLICK: //单击
				{
					for (auto& fncn : FNACN) {
						ShowWindow(fncn.groupBox1, SW_HIDE);
						ShowWindow(fncn.groupBox2, SW_HIDE);
						ShowWindow(fncn.hPOSEdit, SW_HIDE);
					}
					ShowWindow(FNACN[activeItem->iItem].groupBox1, SW_SHOW);
					ShowWindow(FNACN[activeItem->iItem].groupBox2, SW_SHOW);
					ShowWindow(FNACN[activeItem->iItem].hPOSEdit, SW_SHOW);

				}	break;
				case NM_DBLCLK:
				{
					//char a[50];
					//_itoa_s(activeItem->iItem, a, 10);	//activeItem->iItem项目序号，将int整型数转化为一个字符串
					//MessageBoxA(hWnd, a, "双击", 0);
				}	break;
				case NM_RCLICK:
				{
					//char a[50];
					//_itoa_s(activeItem->iItem, a, 10);
					//MessageBoxA(hWnd, a, "右击", 0);
				}   break;
				}
			}

		}break;
		}
	}break;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))// 分析wParam低位
		{
		case ID_BUTTON_SAVE:
		{
			save();

			MessageBox(hWnd, L"保存", L"提示", MB_OK);
		}	
		break;
		case ID_BUTTON_CANCEL:
		{
			//DestroyWindow(hWnd);//销毁当前窗口
			closeWindow();
		}	
		break;
		}
	}break;
	case WM_DESTROY: {		//关闭窗口时总是处理
		xWindowModule::postQuitMessage();
		return 0;
	}break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void SettingModelWindow::save()
{
	TCHAR tchar[1000];
	for (auto& fncn : FNACN) {
		auto fileName = STW_U8(fncn.fileName);

		std::wstring wstr;
		for (auto handle : fncn.checkBoxHandles) {
			if (SendMessage(handle, BM_GETCHECK, 0, 0) == TRUE)//是否打勾了 
			{
				GetWindowText(handle, tchar, 1000);
				std::wstring temp = tchar;
				wstr = wstr + L"," + temp;
			}
		}
		IniFile::getInstance()->SetProfileStringWithPath(fileName.c_str(), L"SelectedColumnNames", wstr.c_str());

		for (auto handle : fncn.radioButtonHandles) {
			if (SendMessage(handle, BM_GETCHECK, 0, 0) == TRUE)
			{
				GetWindowText(handle, tchar, 1000);
				break;
			}
		}
		IniFile::getInstance()->SetProfileStringWithPath(fileName.c_str(), L"WordColumnName", tchar);

		GetWindowText(fncn.hPOSEdit, tchar, 1000);
		IniFile::getInstance()->SetProfileStringWithPath(fileName.c_str(), L"PartOfSpeech", tchar);
	}
	
	///////////////////////////////////////////////////////////////////////////
	//设置词典名无需指定段名，内部已默认指定目录
	TCHAR buff[80] = _T("");
	GetWindowText(hDicNameEdit, buff, 80);
	IniFile::getInstance()->SetProfileStringWithPath(L"", L"CustomDicName", buff);
	
	///////////////////////////////////////////////////////////////////////////
	SearchDictionaryWindow* ptr = (SearchDictionaryWindow*)parentWindowPtr;
	ptr->reader->setSelectedColumnNamesAndWordColumnName();

}
