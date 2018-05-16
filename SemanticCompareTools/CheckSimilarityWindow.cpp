#include "stdafx.h"
#include "CheckSimilarityWindow.h"

#include "windowClassRegister.h"
#include "xWindowModule.h"

#include "IniFile.h"
#include "csReader.h"
#include "csRecorder.h"

#include <set>

extern xWindowModule winModule;

// 静态数据成员必须在类定义 外 进行初始化
WNDPROC CheckSimilarityWindow::oldSearchEditProc = NULL;
static CheckSimilarityWindow* myWindow;


CheckSimilarityWindow::CheckSimilarityWindow(HINSTANCE instance) : xWindow(instance)
{
	wcscpy_s(className, L"CheckSimilarityWindow");
	wcscpy_s(title, L"词义对比");
		
	reader = new csReader();
	recorder = new csRecorder();
	recorder->Init();
}

CheckSimilarityWindow::~CheckSimilarityWindow()
{
	delete reader;
	delete recorder;
}

void CheckSimilarityWindow::registerClass()
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
	wcex.lpszMenuName = MAKEINTRESOURCEW(ID_MENU_CHECKSIMILARITY);
	wcex.lpszClassName = className;
	wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

	WindowClassRegister::getInstance()->registerClass(&wcex);
}

void CheckSimilarityWindow::initWindow()
{
	winModule.AddXWindowObject(nullptr, this);

	hWnd = CreateWindowW(className, title, WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX,
		CW_USEDEFAULT, 0, 800, 450, nullptr, nullptr, hInstance, nullptr);

	//////////////////////////////////////////////////////////////////////
	std::vector<std::string> columnNames{
		u8"gkb_词语",
		u8"gkb_词类",
		u8"gkb_拼音",
		u8"gkb_同形",
		u8"gkb_释义",
		u8"gkb_例句",
		u8"ID",
		u8"词语",
		u8"义项编码",
		u8"拼音",
		u8"词性",
		u8"义项释义",
		u8"示例",
		u8"相似度"
	};
	reader->setColumnNames(columnNames);
	reader->setWordColumnName(u8"gkb_词语");

	//////////////////////////////////////////////////////////////////////
	//初始化搜索编辑框
	myWindow = this;

	hSearchEdit = CreateWindow(_T("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT,
		70, 10, 200, 30, hWnd, (HMENU)ID_EDIT_SEARCH, hInstance, NULL);
	//设置处理过程
	CheckSimilarityWindow::oldSearchEditProc = 
		(WNDPROC)SetWindowLongPtr(hSearchEdit, GWLP_WNDPROC, (LONG_PTR)CheckSimilarityWindow::searchEditProc);

	//////////////////////////////////////////////////////////////////////
	//初始化搜索按钮
	hSearchButton = CreateWindow(_T("BUTTON"), _T("搜索"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		310, 10, 100, 30, hWnd, (HMENU)ID_BUTTON_SEARCH, hInstance, NULL);

	//////////////////////////////////////////////////////////////////////
	//初始化类别下拉列表
	hClassComboBox = CreateWindow(WC_COMBOBOX, _T(""), CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		600, 10, 100, 500, hWnd, (HMENU)ID_COMBOBOX_SEARCH, hInstance, NULL);

	// load the combobox with item list. Send a CB_ADDSTRING message to load each item
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"r");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"v");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"d");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"p");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"c");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"q");

	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"n");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"o");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"m");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"e");

	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"未");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"a");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"u");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"缀");

	// Send the CB_SETCURSEL message to display an initial item in the selection field  
	SendMessage(hClassComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	//////////////////////////////////////////////////////////////////////
	//初始化文本	
	hWordText = CreateWindow(_T("static"), _T("词语"), WS_CHILD | WS_VISIBLE | SS_LEFT, 30, 15, 30, 30, hWnd,
		NULL, hInstance, NULL);

	hPosText = CreateWindow(_T("static"), _T("选择词类"), WS_CHILD | WS_VISIBLE | SS_LEFT, 500, 15, 80, 30, hWnd,
		NULL, hInstance, NULL);

	hDicText_One = CreateWindow(_T("static"), _T("GKB现代汉语语法信息词典"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		30, 47, 300, 30, hWnd,	NULL, hInstance, NULL);

	hDicText_Two = CreateWindow(_T("static"), _T("现代汉语词典"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		30, 135, 300, 30, hWnd,	NULL, hInstance, NULL);

	HFONT hFont = CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"微软雅黑");//创建字体
	SendMessage(hSearchButton, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(hSearchEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hWordText, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hPosText, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hDicText_One, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hDicText_Two, WM_SETFONT, (WPARAM)hFont, TRUE);
	
	{
		//////////////////////////////////////////////////////////////////////
		//初始化词典1的列表视图
		hDictionaryListView_One = CreateWindowW(WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_NOSORTHEADER,
			25, 70, 740, 65, hWnd, (HMENU)ID_LISTVIEW_DICTIONARY_ONE, hInstance, NULL);

		WCHAR szText[256];     // Temporary buffer.
		LVCOLUMN lvc;
		// Initialize the LVCOLUMN structure.
		// The mask specifies that the format, width, text,
		// and subitem members of the structure are valid.
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvc.fmt = LVCFMT_LEFT;
		//Add the columns.

		lvc.iSubItem = 0;
		lvc.pszText = szText;
		lvc.cx = 60;
		wcscpy_s(szText, L"词语");
		if (ListView_InsertColumn(hDictionaryListView_One, 0, &lvc) == -1)
			return;
		
		lvc.iSubItem = 1;
		lvc.pszText = szText;
		lvc.cx = 40;
		wcscpy_s(szText, L"词类");
		if (ListView_InsertColumn(hDictionaryListView_One, 1, &lvc) == -1)
			return;
		
		lvc.iSubItem = 2;
		lvc.pszText = szText;
		lvc.cx = 100;
		wcscpy_s(szText, L"拼音");
		if (ListView_InsertColumn(hDictionaryListView_One, 2, &lvc) == -1)
			return;
		
		lvc.iSubItem = 3;
		lvc.pszText = szText;
		lvc.cx = 40;
		wcscpy_s(szText, L"同形");
		if (ListView_InsertColumn(hDictionaryListView_One, 3, &lvc) == -1)
			return;
		
		lvc.iSubItem = 4;
		lvc.pszText = szText;
		lvc.cx = 245;
		wcscpy_s(szText, L"释义");
		if (ListView_InsertColumn(hDictionaryListView_One, 4, &lvc) == -1)
			return;
		
		lvc.iSubItem = 5;
		lvc.pszText = szText;
		lvc.cx = 255;
		wcscpy_s(szText, L"例句");
		if (ListView_InsertColumn(hDictionaryListView_One, 5, &lvc) == -1)
			return;
			
		//////////////////////////////////////////////////////////////////////
		//初始化词典2的列表视图
		hDictionaryListView_Two = CreateWindowW(WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | WS_BORDER |
			LVS_REPORT | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS,
			25, 160, 740, 180, hWnd, (HMENU)ID_LISTVIEW_DICTIONARY_TWO, hInstance, NULL);

		ListView_SetExtendedListViewStyle(hDictionaryListView_Two, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);      //设置整行选择风格
		
		lvc.iSubItem = 0;
		lvc.pszText = szText;
		lvc.cx = 70;
		wcscpy_s(szText, L"ID");
		if (ListView_InsertColumn(hDictionaryListView_Two, 0, &lvc) == -1)
			return;

		lvc.iSubItem = 1;
		lvc.pszText = szText;
		lvc.cx = 60;
		wcscpy_s(szText, L"词语");
		if (ListView_InsertColumn(hDictionaryListView_Two, 1, &lvc) == -1)
			return;

		lvc.iSubItem = 2;
		lvc.pszText = szText;
		lvc.cx = 90;
		wcscpy_s(szText, L"义项编码");
		if (ListView_InsertColumn(hDictionaryListView_Two, 2, &lvc) == -1)
			return;

		lvc.iSubItem = 3;
		lvc.pszText = szText;
		lvc.cx = 75;
		wcscpy_s(szText, L"拼音");
		if (ListView_InsertColumn(hDictionaryListView_Two, 3, &lvc) == -1)
			return;
		
		lvc.iSubItem = 4;
		lvc.pszText = szText;
		lvc.cx = 40;
		wcscpy_s(szText, L"词性");
		if (ListView_InsertColumn(hDictionaryListView_Two, 4, &lvc) == -1)
			return;
		
		lvc.iSubItem = 5;
		lvc.pszText = szText;
		lvc.cx = 170;
		wcscpy_s(szText, L"义项释义");
		if (ListView_InsertColumn(hDictionaryListView_Two, 5, &lvc) == -1)
			return;

		lvc.iSubItem = 6;
		lvc.pszText = szText;
		lvc.cx = 180;
		wcscpy_s(szText, L"示例");
		if (ListView_InsertColumn(hDictionaryListView_Two, 6, &lvc) == -1)
			return;

		lvc.iSubItem = 7;
		lvc.pszText = szText;
		lvc.cx = 50;
		wcscpy_s(szText, L"相似度");
		if (ListView_InsertColumn(hDictionaryListView_Two, 7, &lvc) == -1)
			return;

	}

	//////////////////////////////////////////////////////////////////////
	//Check按钮
	hCheckButton = CreateWindow(_T("BUTTON"), _T("Check"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		60, 350, 100, 30, hWnd, (HMENU)ID_BUTTON_CHECK, hInstance, NULL);

	//////////////////////////////////////////////////////////////////////
	//“上一”按钮
	hPrevWordButton = CreateWindow(_T("BUTTON"), _T("上一个"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		250, 350, 100, 30, hWnd, (HMENU)ID_BUTTON_PREV_WORD, hInstance, NULL);

	//////////////////////////////////////////////////////////////////////
	//“下一”按钮
	hNextWordButton = CreateWindow(_T("BUTTON"), _T("下一个"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		450, 350, 100, 30, hWnd, (HMENU)ID_BUTTON_NEXT_WORD, hInstance, NULL);
	
	//////////////////////////////////////////////////////////////////////
	// 禁用某些功能，直到被激活，TRUE使用，FALSE禁止
	activeControls(FALSE);
}

BOOL CheckSimilarityWindow::activeControls(bool boolean)
{
	EnableWindow(hSearchButton, boolean);
	EnableWindow(hSearchEdit, boolean);
	EnableWindow(hDictionaryListView_One, boolean);
	EnableWindow(hDictionaryListView_Two, boolean);
	EnableWindow(hCheckButton, boolean);
	EnableWindow(hPrevWordButton, boolean);
	EnableWindow(hNextWordButton, boolean);

	return true;
}

void CheckSimilarityWindow::search()
{
	TCHAR buff[80] = _T("");
	GetWindowText(hSearchEdit, buff, 80);
	std::string& word = WTS_U8(buff);

	if (reader->findWord(word)) {
		refreshMainWindow();
	}
	else {
		MessageBox(hWnd, L"在当前词类下未找到该词语。", L"提示", MB_OK);
	}

}

void CheckSimilarityWindow::openFiles()
{
	//调用 shell32.dll api   调用浏览文件夹对话框 
	TCHAR szPathName[MAX_PATH];
	BROWSEINFO bInfo = { 0 };
	bInfo.hwndOwner = GetForegroundWindow();//父窗口  
	bInfo.lpszTitle = TEXT("请选择词义对应表所在的文件夹");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI/*包含一个编辑框 用户可以手动填写路径 对话框可以调整大小之类的..*/ |
		BIF_UAHINT/*带TIPS提示*/ | BIF_NONEWFOLDERBUTTON /*不带新建文件夹按钮*/;
	LPITEMIDLIST lpDlist;
	lpDlist = SHBrowseForFolder(&bInfo);
	if (lpDlist != NULL)//单击了确定按钮  
	{
		////////////////////////////////////////////////////////////////////
		// 获取目录，为了支持中文，需要将获取的字符串的TCHAR编码转为CHAR编码，使用代码页CP_ACP
		SHGetPathFromIDList(lpDlist, szPathName);

		char cPath[256];
		//获取字节长度   
		int iLength = WideCharToMultiByte(CP_ACP, 0, szPathName, -1, NULL, 0, NULL, NULL);
		//将tchar值赋给_char    
		WideCharToMultiByte(CP_ACP, 0, szPathName, -1, cPath, iLength, NULL, NULL);
		
		////////////////////////////////////////////////////////////////////
		IniFile::getInstance()->SetFilePath(szPathName);

		szFilePath = szPathName;

		////////////////////////////////////////////////////////////////////
		//搜索目录下的xlsx文件
		std::string searchPath = cPath;
		searchPath = searchPath + "\\*.xlsx";

		std::wstring wpath = szPathName;
		wpath = wpath + L"\\";

		_finddata_t fileDir;
		long lfDir;
		if ((lfDir = _findfirst(searchPath.c_str(), &fileDir)) == -1l) {
			MessageBox(hWnd, L"No xlsx file is found\n", L"提示", MB_OK);
			return;
		}
		else {
			//首先清除之前载入的所有信息
			reader->clear();

			do {
				//找到了xlsx文件，先将char转换为wchar，再将wchar转换为utf-8编码的char
				TCHAR cache[256];
				iLength = MultiByteToWideChar(CP_ACP, 0, fileDir.name, strlen(fileDir.name) + 1, NULL, 0);
				MultiByteToWideChar(CP_ACP, 0, fileDir.name, strlen(fileDir.name) + 1, cache, iLength);

				std::wstring ws(cache);
				std::string utf8_str = WTS_U8(ws);

				reader->addXlsxFileName(utf8_str);

			} while (_findnext(lfDir, &fileDir) == 0);
		}
		_findclose(lfDir);

	}

}

void CheckSimilarityWindow::loadFiles()
{
	//传入正则表达式，以及代表的词类英文字母
	std::wstring pattern_1to1{ L"\\w*\\dto\\d\\w*" };
	std::wstring pattern_1tom{ L"\\w*\\dto[a-zA-Z]\\w*" };
	std::wstring pattern_mto1{ L"\\w*[a-zA-Z]to\\d\\w*" };
	std::wstring pattern_mtom{ L"\\w*[a-zA-Z]to[a-zA-Z]\\w*" };
	std::wstring pattern_end{ L"\\w*.xlsx" };

	std::vector<std::wstring> pattern_part;
	pattern_part.push_back(pattern_1to1);
	pattern_part.push_back(pattern_1tom);
	pattern_part.push_back(pattern_mto1);
	pattern_part.push_back(pattern_mtom);

	std::wstring wpath = szFilePath;
	wpath = wpath + L"\\";

	for (auto& part : pattern_part) {
		reader->loadXlsxFile(WTS_U8(part + L"代" + pattern_end), WTS_U8(PART_OF_SPEECH_DAI), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"动" + pattern_end), WTS_U8(PART_OF_SPEECH_DONG), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"副" + pattern_end), WTS_U8(PART_OF_SPEECH_FU), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"介" + pattern_end), WTS_U8(PART_OF_SPEECH_JIE), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"连" + pattern_end), WTS_U8(PART_OF_SPEECH_LIAN), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"量" + pattern_end), WTS_U8(PART_OF_SPEECH_LIANG), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"名" + pattern_end), WTS_U8(PART_OF_SPEECH_MING), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"拟" + pattern_end), WTS_U8(PART_OF_SPEECH_NI), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"数" + pattern_end), WTS_U8(PART_OF_SPEECH_SHU), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"叹" + pattern_end), WTS_U8(PART_OF_SPEECH_TAN), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"未" + pattern_end), WTS_U8(PART_OF_SPEECH_WEI), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"形" + pattern_end), WTS_U8(PART_OF_SPEECH_XING), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"助" + pattern_end), WTS_U8(PART_OF_SPEECH_ZHU), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"缀" + pattern_end), WTS_U8(PART_OF_SPEECH_ZHUI), WTS_U8(wpath));
	}

}

void CheckSimilarityWindow::next()
{
	if (reader->toNextIsomorphicOfCurWord()) {
		refreshMainWindow();	
	}
	else if (reader->nextWord()) {
		refreshMainWindow();
	}
	else
		MessageBox(hWnd, L"已是最后一个词语。", L"提示", MB_OK);
}

void CheckSimilarityWindow::prev()
{
	if (reader->toPrevIsomorphicOfCurWord() || reader->prevWord()) {	//短路求值
		refreshMainWindow();
	}
	else
		MessageBox(hWnd, L"已是第一个词语。", L"提示", MB_OK);
}

void CheckSimilarityWindow::refreshListView()
{
	// 清除ListView中的所有项 
	ListView_DeleteAllItems(hDictionaryListView_One);
	ListView_DeleteAllItems(hDictionaryListView_Two);

	std::pair<unsigned int, std::vector<unsigned int>> neededRows = reader->getRowsByIsomorphicIndex();

	rowOfWord = neededRows.first;

	LVITEM vitem;
	vitem.mask = LVIF_TEXT;
	vitem.iItem = 0;

	std::wstring str;//临时变量str保存返回的宽字符字符串	
					 //////////////////////////////////////////////////////////////////////
					 //词典1	
	str = stringToWstring(reader->getValueInColumnByRow(neededRows.first, u8"gkb_词语"));
	vitem.iSubItem = 0;
	vitem.pszText = (LPWSTR)str.c_str();
	ListView_InsertItem(hDictionaryListView_One, &vitem);//区别insert和set

	str = stringToWstring(reader->getValueInColumnByRow(neededRows.first, u8"gkb_词类"));
	vitem.iSubItem = 1;
	vitem.pszText = (LPWSTR)str.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);

	str = stringToWstring(reader->getValueInColumnByRow(neededRows.first, u8"gkb_拼音"));
	vitem.iSubItem = 2;
	vitem.pszText = (LPWSTR)str.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);

	str = stringToWstring(reader->getValueInColumnByRow(neededRows.first, u8"gkb_同形"));
	vitem.iSubItem = 3;
	vitem.pszText = (LPWSTR)str.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);

	str = stringToWstring(reader->getValueInColumnByRow(neededRows.first, u8"gkb_释义"));
	vitem.iSubItem = 4;
	vitem.pszText = (LPWSTR)str.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);

	str = stringToWstring(reader->getValueInColumnByRow(neededRows.first, u8"gkb_例句"));
	vitem.iSubItem = 5;
	vitem.pszText = (LPWSTR)str.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);

	////////////////////////////////////////////////////////////////////////
	//词典2
	numberOfItem = neededRows.second.size();

	for (int i = 0; i < numberOfItem; i++)
	{
		unsigned int row = neededRows.second[i];

		vitem.iItem = i;

		str = stringToWstring(reader->getValueInColumnByRow(row, u8"ID"));
		vitem.iSubItem = 0;
		vitem.pszText = (LPWSTR)str.c_str();
		ListView_InsertItem(hDictionaryListView_Two, &vitem);

		str = stringToWstring(reader->getValueInColumnByRow(row, u8"词语"));
		vitem.iSubItem = 1;
		vitem.pszText = (LPWSTR)str.c_str();
		ListView_SetItem(hDictionaryListView_Two, &vitem);

		str = stringToWstring(reader->getValueInColumnByRow(row, u8"义项编码"));
		vitem.iSubItem = 2;
		vitem.pszText = (LPWSTR)str.c_str();
		ListView_SetItem(hDictionaryListView_Two, &vitem);

		str = stringToWstring(reader->getValueInColumnByRow(row, u8"拼音"));
		vitem.iSubItem = 3;
		vitem.pszText = (LPWSTR)str.c_str();
		ListView_SetItem(hDictionaryListView_Two, &vitem);

		str = stringToWstring(reader->getValueInColumnByRow(row, u8"词性"));
		vitem.iSubItem = 4;
		vitem.pszText = (LPWSTR)str.c_str();
		ListView_SetItem(hDictionaryListView_Two, &vitem);

		str = stringToWstring(reader->getValueInColumnByRow(row, u8"义项释义"));
		vitem.iSubItem = 5;
		vitem.pszText = (LPWSTR)str.c_str();
		ListView_SetItem(hDictionaryListView_Two, &vitem);

		str = stringToWstring(reader->getValueInColumnByRow(row, u8"示例"));
		vitem.iSubItem = 6;
		vitem.pszText = (LPWSTR)str.c_str();
		ListView_SetItem(hDictionaryListView_Two, &vitem);

		str = stringToWstring(reader->getValueInColumnByRow(row, u8"相似度"));
		vitem.iSubItem = 7;
		vitem.pszText = (LPWSTR)str.c_str();
		ListView_SetItem(hDictionaryListView_Two, &vitem);
	}


}

void CheckSimilarityWindow::resetPartOfSpeech()
{
	// If the user makes a selection from the list:
	//   Send CB_GETCURSEL message to get the index of the selected list item.
	//   Send CB_GETLBTEXT message to get the item.	
	//获取当前列表框中的选项索引
	int ItemIndex = SendMessage(hClassComboBox, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	//获取当前列表框中的选项值	
	TCHAR  part_of_speech[256];
	(TCHAR)SendMessage(hClassComboBox, (UINT)CB_GETLBTEXT, (WPARAM)ItemIndex, (LPARAM)part_of_speech);

	reader->setPartOfSpeech(WTS_U8(part_of_speech));

}

void CheckSimilarityWindow::insertAllRecord()
{

	do
	{
		std::pair<unsigned int, std::vector<unsigned int>>& neededRows = reader->getRowsByIsomorphicIndex();
		
		std::string& word = reader->getValueInColumnByRow(neededRows.first, u8"gkb_词语");
		std::string& pos = reader->getValueInColumnByRow(neededRows.first, u8"gkb_词类");
		std::string& alphabetic = reader->getValueInColumnByRow(neededRows.first, u8"gkb_拼音");
		std::string& isomorphic = reader->getValueInColumnByRow(neededRows.first, u8"gkb_同形");
		std::string& meanings = reader->getValueInColumnByRow(neededRows.first, u8"gkb_释义");
		std::string& example = reader->getValueInColumnByRow(neededRows.first, u8"gkb_例句");

		//从记录中寻找是否存在完全符合的词语
		if (recorder->findRecord(word, pos, alphabetic, isomorphic, meanings, example))
		{
			continue;
		}
		//没有找到相同记录，自动判断相似度大小关系，添加新纪录
		else
		{
			std::vector<std::pair<std::string,double>> ids_sims;
			for (auto& row : neededRows.second) 
			{
				std::string& id = reader->getValueInColumnByRow(row, u8"ID");
				std::string& sim = reader->getValueInColumnByRow(row, u8"相似度");

				if (sim == "")
					ids_sims.push_back(std::make_pair(id, 0.00));
				else
					ids_sims.push_back(std::make_pair(id, std::stod(sim) ) );
			}



			std::vector<std::string> biggestSimId;
			double biggest = 0.0;

			for (auto& id_sim : ids_sims) 
			{
				double similarity = id_sim.second;

				if (similarity > biggest) {
					biggestSimId.clear();
					biggest = similarity;
					biggestSimId.push_back(id_sim.first);
				}
				else if (similarity == biggest) {
					biggestSimId.push_back(id_sim.first);
				}
			}

			//插入新的记录
			std::vector<std::string> vector;			
			vector.push_back(word);
			vector.push_back(pos);
			vector.push_back(alphabetic);
			vector.push_back(isomorphic);
			vector.push_back(meanings);
			vector.push_back(example);

			std::set<std::string> idSet;
			for (auto& id : biggestSimId) {
				idSet.insert(id);
			}
			std::vector<std::string> ids;
			for (auto& id : idSet) {
				ids.push_back(id);
			}
			recorder->insertNewRecord(vector, ids);
		}
	} while (reader->toNextIsomorphicOfCurWord() || reader->nextWord());

}

void CheckSimilarityWindow::setCheckState()
{
	//获取当前GKB列表中的词语信息
	TCHAR buf[256];
	ListView_GetItemText(hDictionaryListView_One, 0, 0, buf, 256);//句柄，item，iSubItem，数组，选取的大小
	std::string word = WTS_U8(buf);
	ListView_GetItemText(hDictionaryListView_One, 0, 1, buf, 256);
	std::string pos = WTS_U8(buf);
	ListView_GetItemText(hDictionaryListView_One, 0, 2, buf, 256);
	std::string alphabetic = WTS_U8(buf);
	ListView_GetItemText(hDictionaryListView_One, 0, 3, buf, 256);
	std::string isomorphic = WTS_U8(buf);
	ListView_GetItemText(hDictionaryListView_One, 0, 4, buf, 256);
	std::string meanings = WTS_U8(buf);
	ListView_GetItemText(hDictionaryListView_One, 0, 5, buf, 256);
	std::string example = WTS_U8(buf);

	//从记录中寻找是否存在完全符合的词语
	if (recorder->findRecord(word, pos, alphabetic, isomorphic, meanings, example))
	{
		std::vector<std::string> IDs = recorder->getIDsAfterFind();

		for (auto& id : IDs)
		{
			for (int i = 0; i < numberOfItem; i++)
			{
				//取ID
				ListView_GetItemText(hDictionaryListView_Two, i, 0, buf, 256);

				if (id == WTS_U8(buf)) {
					ListView_SetCheckState(hDictionaryListView_Two, i, TRUE);
					break;
				}

			}
		}

	}

	//没有找到相同记录，自动判断相似度大小关系，自动打勾，取打勾结果，添加新纪录
	//else if (numberOfItem > 0)
	//{
	//	ListView_GetItemText(hDictionaryListView_Two, 0, 7, buf, 256);
	//	std::wstring wstr = buf;
	//	double biggest;
	//	if (wstr == L"") {
	//		biggest = 0.0;
	//	}
	//	else {
	//		biggest = std::stod(buf);

	//	}

	//	//取首个相似度
	//	std::vector<unsigned int> indexToCheck;
	//	indexToCheck.push_back(0);

	//	for (unsigned int i = 1; i < numberOfItem; i++)
	//	{
	//		ListView_GetItemText(hDictionaryListView_Two, i, 7, buf, 256);
	//		std::wstring wstr = buf;

	//		if (wstr == L"") {
	//			continue;
	//		}
	//		else {
	//			double similarity;
	//			similarity = std::stod(buf);
	//			if (similarity > biggest)
	//			{
	//				indexToCheck.clear();
	//				biggest = similarity;
	//				indexToCheck.push_back(i);
	//			}
	//			else if (similarity == biggest) {
	//				indexToCheck.push_back(i);
	//			}

	//		}
	//	}

	//	for (unsigned int index : indexToCheck) {
	//		ListView_SetCheckState(hDictionaryListView_Two, index, TRUE);
	//	}

	//	//////////////////////////////////////////////////////////////
	//	//存储选取的id
	//	std::vector<std::string> ids;
	//	for (unsigned int i = 0; i < numberOfItem; i++)
	//	{
	//		if (ListView_GetCheckState(hDictionaryListView_Two, i)) {

	//			TCHAR buffer[256];
	//			ListView_GetItemText(hDictionaryListView_Two, i, 0, buffer, 256);

	//			ids.push_back(WTS_U8(buffer));
	//		}
	//	}

	//	//插入新的记录
	//	std::vector<std::string> vector;
	//	std::string str;
	//	str = reader->getValueInColumnByRow(rowOfWord, u8"gkb_词语");
	//	vector.push_back(str);
	//	str = reader->getValueInColumnByRow(rowOfWord, u8"gkb_词类");
	//	vector.push_back(str);
	//	str = reader->getValueInColumnByRow(rowOfWord, u8"gkb_拼音");
	//	vector.push_back(str);
	//	str = reader->getValueInColumnByRow(rowOfWord, u8"gkb_同形");
	//	vector.push_back(str);
	//	str = reader->getValueInColumnByRow(rowOfWord, u8"gkb_释义");
	//	vector.push_back(str);
	//	str = reader->getValueInColumnByRow(rowOfWord, u8"gkb_例句");
	//	vector.push_back(str);

	//	recorder->insertNewRecord(vector, ids);
	//}


}

void CheckSimilarityWindow::check()
{
	//存储选取的id，使用string类型记录，虽然效率会差很多
	std::vector<std::string> ids;
	for (int i = 0; i < numberOfItem; i++)
	{
		if (ListView_GetCheckState(hDictionaryListView_Two, i)) {

			TCHAR buf[256];
			ListView_GetItemText(hDictionaryListView_Two, i, 0, buf, 256);

			ids.push_back(WTS_U8(buf));
			//MessageBox(hWnd, buf, L"提示", MB_OK);
		}
	}

	//获取当前GKB列表中的词语信息
	TCHAR buf[256];
	ListView_GetItemText(hDictionaryListView_One, 0, 0, buf, 256);//句柄，item，iSubItem，数组，选取的大小
	std::string word = WTS_U8(buf);
	ListView_GetItemText(hDictionaryListView_One, 0, 1, buf, 256);
	std::string pos = WTS_U8(buf);
	ListView_GetItemText(hDictionaryListView_One, 0, 2, buf, 256);
	std::string alphabetic = WTS_U8(buf);
	ListView_GetItemText(hDictionaryListView_One, 0, 3, buf, 256);
	std::string isomorphic = WTS_U8(buf);
	ListView_GetItemText(hDictionaryListView_One, 0, 4, buf, 256);
	std::string meanings = WTS_U8(buf);
	ListView_GetItemText(hDictionaryListView_One, 0, 5, buf, 256);
	std::string example = WTS_U8(buf);

	//从记录中寻找是否存在完全符合的词语
	if (recorder->findRecord(word, pos, alphabetic, isomorphic, meanings, example))
	{
		recorder->updateIDs(ids);
	}
}

void CheckSimilarityWindow::refreshMainWindow()
{
	refreshListView();
	setCheckState();
}

LRESULT CheckSimilarityWindow::processMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{		
		// 分析wParam高位: 
		switch (HIWORD(wParam))
		{
		case CBN_SELCHANGE:
		{
			//只有当读取器已经加载过文件时，才自动刷新主窗口
			//此时才需要替换词类，打开文件时也会根据列表内容再选择一次词类
			if (reader->isExistingFile()) {
				resetPartOfSpeech();

				std::pair<unsigned int, std::vector<unsigned int>>& neededRows = reader->getRowsByIsomorphicIndex();
				numberOfItem = neededRows.second.size();

				if (numberOfItem == 0) {
					refreshMainWindow();
				}
				else {
					insertAllRecord();

					resetPartOfSpeech();
					refreshMainWindow();
				}

			}
		}
		break;
		default:
			//继续处理
			break;
		}

		// 分析wParam低位: 
		switch (LOWORD(wParam))
		{
		case ID_BUTTON_SEARCH:
		{
			search();
		}
		break;
		case ID_BUTTON_CHECK:
		{
			if (numberOfItem > 0) {
				check();			
			}
		}
		break;
		case ID_BUTTON_PREV_WORD:
		{
			if (reader->isExistingFile()) {
				prev();
			}
		}
		break;
		case ID_BUTTON_NEXT_WORD:
		{
			if (reader->isExistingFile()) {
				next();
			}
		}
		break;
		case ID_MENUITEM_OPEN:
		{
			openFiles();
			loadFiles();

			if (reader->isExistingFile()) {
				// 激活某些控件
				activeControls(TRUE);

				// 打开文件后默认显示当前词类选择列表中指定的表格，刷新主窗口
				// 这样当未打开文件时，也可以随便选择词类，但是不会产生效果
				resetPartOfSpeech();

				std::pair<unsigned int, std::vector<unsigned int>>& neededRows = reader->getRowsByIsomorphicIndex();
				numberOfItem = neededRows.second.size();

				if (numberOfItem == 0) {
					refreshMainWindow();					
				}
				else {
					insertAllRecord();

					resetPartOfSpeech();
					refreshMainWindow();
				}

			}

		}
		break;
		case ID_MENUITEM_SAVE:
		{
			recorder->save();
			MessageBox(hWnd, L"已保存。", L"提示", MB_OK);
		}
		break;		
		case ID_MENUITEM_EXIT:
			closeWindow();
		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

	}
	break;
	case WM_SIZE: {
		int nHeight = HIWORD(lParam);
		int nWidth = LOWORD(lParam);

		int heightInc = nHeight - 450;
		int widthInc = nWidth - 800;

		MoveWindow(hDictionaryListView_One, 25, 70, 750 + widthInc, 65, TRUE);
		MoveWindow(hDictionaryListView_Two, 25, 160, 750 + widthInc, 180 , TRUE);
	}
	break;
	case WM_GETMINMAXINFO: {
		MINMAXINFO * pMinMaxInfo = (MINMAXINFO *)lParam;
		pMinMaxInfo->ptMinTrackSize.x = 810;
		pMinMaxInfo->ptMinTrackSize.y = 450;
		pMinMaxInfo->ptMaxTrackSize.x = 1600;
		pMinMaxInfo->ptMaxTrackSize.y = 450;
		return 0;
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...
		//
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
	{
		recorder->save();

		xWindowModule::postQuitMessage();
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	
	return 0;
}

LRESULT CheckSimilarityWindow::searchEditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
			myWindow->search();

			break;  //or return 0; if you don't want to pass it further to def proc
					//If not your key, skip to default:
		}
	default:
		return CallWindowProc(CheckSimilarityWindow::oldSearchEditProc, hWnd, msg, wParam, lParam);
	}
	return 0;
}
