#include "stdafx.h"
#include "SenseAlignmentWindow.h"

#include "windowClassRegister.h"
#include "xWindowModule.h"

#include "IniFile.h"
#include "saReader.h"
#include "saRecorder.h"

extern xWindowModule winModule;

// 静态数据成员必须在类定义 外 进行初始化
WNDPROC SenseAlignmentWindow::oldSearchEditProc = NULL;
static SenseAlignmentWindow* myWindow;

SenseAlignmentWindow::SenseAlignmentWindow(HINSTANCE instance) : xWindow(instance)
{
	wcscpy_s(className, L"SenseAlignmentWindow");
	wcscpy_s(title, L"词义对比");

	reader = new saReader();
	recorder = new saRecorder();
	recorder->Init();
}

SenseAlignmentWindow::~SenseAlignmentWindow()
{
	delete reader;
	delete recorder;
}

void SenseAlignmentWindow::registerClass()
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

void SenseAlignmentWindow::initWindow()
{
	winModule.AddXWindowObject(nullptr, this);

	hWnd = CreateWindowW(className, title, WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX,
		CW_USEDEFAULT, 0, 800, 450, nullptr, nullptr, hInstance, nullptr);

	//////////////////////////////////////////////////////////////////////
	std::vector<std::string> columnNames{
		u8"ID",
		u8"词语",
		u8"义项编码",
		u8"拼音",
		u8"词性",
		u8"义项释义",
		u8"示例",
		u8"相似度",	
		u8"gkb_词语",
		u8"gkb_词类",
		u8"gkb_拼音",
		u8"gkb_同形",
		u8"gkb_释义",
		u8"gkb_例句",
		u8"映射关系",
	};
	reader->setColumnNames(columnNames);
	reader->setWordColumnName(u8"词语");

	//////////////////////////////////////////////////////////////////////
	//初始化搜索编辑框
	myWindow = this;
	hSearchEdit = CreateWindow(_T("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT,
		65, 20, 180, 30, hWnd, (HMENU)ID_EDIT_SEARCH, hInstance, NULL);
	SenseAlignmentWindow::oldSearchEditProc =
		(WNDPROC)SetWindowLongPtr(hSearchEdit, GWLP_WNDPROC, (LONG_PTR)SenseAlignmentWindow::searchEditProc);

	//////////////////////////////////////////////////////////////////////
	//初始化搜索按钮
	hSearchButton = CreateWindow(_T("BUTTON"), _T("搜索"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		270, 20, 100, 30, hWnd, (HMENU)ID_BUTTON_SEARCH, hInstance, NULL);

	//////////////////////////////////////////////////////////////////////
	//初始化搜索类别下拉列表
	hClassComboBox = CreateWindow(WC_COMBOBOX, _T(""), CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		430, 20, 100, 500, hWnd, (HMENU)ID_COMBOBOX_SEARCH, hInstance, NULL);

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
	HWND wordStaticText = CreateWindow(_T("static"), _T("词语"), WS_CHILD | WS_VISIBLE | SS_LEFT, 25, 20, 30, 30, hWnd,
		NULL, hInstance, NULL);
	HWND posStaticText = CreateWindow(_T("static"), _T("词类"), WS_CHILD | WS_VISIBLE | SS_LEFT, 390, 20, 30, 30, hWnd,
		NULL, hInstance, NULL);

	HWND dic1StaticText = CreateWindow(_T("static"), _T("现代汉语词典"), WS_CHILD | WS_VISIBLE | SS_LEFT, 30, 90, 250, 30, hWnd,
		NULL, hInstance, NULL);
	HWND dic2StaticText = CreateWindow(_T("static"), _T("GKB现代汉语语法信息词典"), WS_CHILD | WS_VISIBLE | SS_LEFT, 30, 200, 250, 30, hWnd,
		NULL, hInstance, NULL);

	hSimilarityStaticText = CreateWindow(_T("static"), _T("相似度"), WS_CHILD | WS_VISIBLE | SS_LEFT, 560, 10, 90, 30, hWnd,
		(HMENU)ID_TEXT_STATIC_SIMILARITY, hInstance, NULL);
	hSimilarityText = CreateWindow(_T("static"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 650, 10, 100, 30, hWnd,
		(HMENU)ID_TEXT_SIMILARITY, hInstance, NULL);

	hRelationshipStaticText = CreateWindow(_T("static"), _T("对应关系"), WS_CHILD | WS_VISIBLE | SS_LEFT, 560, 40, 90, 30, hWnd,
		(HMENU)ID_TEXT_STATIC_RELATIONSHIP, hInstance, NULL);
	hRelationshipText = CreateWindow(_T("static"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 650, 40, 100, 30, hWnd,
		(HMENU)ID_TEXT_RELATIONSHIP, hInstance, NULL);

	hNewRelationshipStaticText = CreateWindow(_T("static"), _T("新对应关系"), WS_CHILD | WS_VISIBLE | SS_LEFT, 560, 70, 90, 30, hWnd,
		(HMENU)ID_TEXT_STATIC_NEW_RELATIONSHIP, hInstance, NULL);
	hNewRelationshipText = CreateWindow(_T("static"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER, 650, 70, 100, 30, hWnd,
		(HMENU)ID_TEXT_NEW_RELATIONSHIP, hInstance, NULL);

	HFONT hFont = CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"微软雅黑");//创建字体
	SendMessage(hSearchButton, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(hSearchEdit, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(hClassComboBox, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息

	SendMessage(wordStaticText, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(posStaticText, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(dic1StaticText, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(dic2StaticText, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息

	SendMessage(hNewRelationshipStaticText, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(hRelationshipStaticText, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(hSimilarityStaticText, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(hNewRelationshipText, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(hRelationshipText, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(hSimilarityText, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	
	{	
		/////////////////////////////////////////////////////////////////////
		//初始化词典1的列表视图
		hDictionaryListView_One = CreateWindow(WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_NOSORTHEADER,
			30, 120, 600, 70, hWnd, (HMENU)ID_LISTVIEW_DICTIONARY_ONE, hInstance, NULL);

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
		wcscpy_s(szText, L"ID");
		if (ListView_InsertColumn(hDictionaryListView_One, 0, &lvc) == -1)
			return;

		lvc.iSubItem = 1;
		lvc.pszText = szText;
		lvc.cx = 60;
		wcscpy_s(szText, L"词语");
		if (ListView_InsertColumn(hDictionaryListView_One, 1, &lvc) == -1)
			return;

		lvc.iSubItem = 2;
		lvc.pszText = szText;
		lvc.cx = 90;
		wcscpy_s(szText, L"义项编码");
		if (ListView_InsertColumn(hDictionaryListView_One, 2, &lvc) == -1)
			return;

		lvc.iSubItem = 3;
		lvc.pszText = szText;
		lvc.cx = 75;
		wcscpy_s(szText, L"拼音");
		if (ListView_InsertColumn(hDictionaryListView_One, 3, &lvc) == -1)
			return;

		lvc.iSubItem = 4;
		lvc.pszText = szText;
		lvc.cx = 40;
		wcscpy_s(szText, L"词性");
		if (ListView_InsertColumn(hDictionaryListView_One, 4, &lvc) == -1)
			return;

		lvc.iSubItem = 5;
		lvc.pszText = szText;
		lvc.cx = 130;
		wcscpy_s(szText, L"义项释义");
		if (ListView_InsertColumn(hDictionaryListView_One, 5, &lvc) == -1)
			return;

		lvc.iSubItem = 6;
		lvc.pszText = szText;
		lvc.cx = 130;
		wcscpy_s(szText, L"示例");
		if (ListView_InsertColumn(hDictionaryListView_One, 6, &lvc) == -1)
			return;

		//////////////////////////////////////////////////////////////////////
		//初始化词典2的列表视图
		hDictionaryListView_Two = CreateWindow(WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_NOSORTHEADER,
			30, 230, 600, 70, hWnd, (HMENU)ID_LISTVIEW_DICTIONARY_TWO, hInstance, NULL);
		
		lvc.iSubItem = 0;
		lvc.pszText = szText;
		lvc.cx = 60;
		wcscpy_s(szText, L"词语");
		if (ListView_InsertColumn(hDictionaryListView_Two, 0, &lvc) == -1)
			return;

		lvc.iSubItem = 1;
		lvc.pszText = szText;
		lvc.cx = 40;
		wcscpy_s(szText, L"词类");
		if (ListView_InsertColumn(hDictionaryListView_Two, 1, &lvc) == -1)
			return;

		lvc.iSubItem = 2;
		lvc.pszText = szText;
		lvc.cx = 75;
		wcscpy_s(szText, L"拼音");
		if (ListView_InsertColumn(hDictionaryListView_Two, 2, &lvc) == -1)
			return;

		lvc.iSubItem = 3;
		lvc.pszText = szText;
		lvc.cx = 40;
		wcscpy_s(szText, L"同形");
		if (ListView_InsertColumn(hDictionaryListView_Two, 3, &lvc) == -1)
			return;

		lvc.iSubItem = 4;
		lvc.pszText = szText;
		lvc.cx = 200;
		wcscpy_s(szText, L"释义");
		if (ListView_InsertColumn(hDictionaryListView_Two, 4, &lvc) == -1)
			return;

		lvc.iSubItem = 5;
		lvc.pszText = szText;
		lvc.cx = 200;
		wcscpy_s(szText, L"例句");
		if (ListView_InsertColumn(hDictionaryListView_Two, 5, &lvc) == -1)
			return;
	}

	//////////////////////////////////////////////////////////////////////
	//按钮
	hRelationEqualButton = CreateWindow(_T("BUTTON"), _T("等于"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		650, 120, 100, 30, hWnd, (HMENU)ID_BUTTON_EQUAL, hInstance, NULL);

	hRelationNotEqualButton = CreateWindow(_T("BUTTON"), _T("不等于"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		650, 170, 100, 30, hWnd, (HMENU)ID_BUTTON_NOT_EQUAL, hInstance, NULL);

	hRelationBelongButton = CreateWindow(_T("BUTTON"), _T("属于"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		650, 220, 100, 30, hWnd, (HMENU)ID_BUTTON_BELONG, hInstance, NULL);

	hRelationUnsureButton = CreateWindow(_T("BUTTON"), _T("不确定"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		650, 270, 100, 30, hWnd, (HMENU)ID_BUTTON_UNSURE, hInstance, NULL);

	hPrevWordButton = CreateWindow(_T("BUTTON"), _T("上一个"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		260, 330, 100, 30, hWnd, (HMENU)ID_BUTTON_PREV_WORD, hInstance, NULL);

	hNextWordButton = CreateWindow(_T("BUTTON"), _T("下一个"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		380, 330, 100, 30, hWnd, (HMENU)ID_BUTTON_NEXT_WORD, hInstance, NULL);
	
	SendMessage(hRelationNotEqualButton, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(hRelationEqualButton, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(hRelationBelongButton, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(hRelationUnsureButton, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(hPrevWordButton, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(hNextWordButton, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	
	//////////////////////////////////////////////////////////////////////
	// 禁用某些功能，直到被激活，TRUE使用，FALSE禁止
	activeControls(FALSE);
}

BOOL SenseAlignmentWindow::activeControls(bool boolean)
{
	EnableWindow(hSearchButton, boolean);
	EnableWindow(hSearchEdit, boolean);
	EnableWindow(hDictionaryListView_One, boolean);
	EnableWindow(hDictionaryListView_Two, boolean);
	EnableWindow(hClassComboBox , boolean);

	EnableWindow(hRelationEqualButton, boolean);
	EnableWindow(hRelationNotEqualButton, boolean);
	EnableWindow(hRelationBelongButton, boolean);
	EnableWindow(hRelationUnsureButton, boolean);

	EnableWindow(hPrevWordButton, boolean);
	EnableWindow(hNextWordButton, boolean);

	return true;
}

void SenseAlignmentWindow::search()
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

void SenseAlignmentWindow::openFiles()
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
		
		////////////////////////////////////////////////////////////////////

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

}

void SenseAlignmentWindow::next()
{
	if (reader->nextWord()) {
		refreshMainWindow();
	}
	else
		MessageBox(hWnd, L"已是最后一个词语。", L"提示", MB_OK);

}

void SenseAlignmentWindow::prev()
{
	if (reader->prevWord()) {
		refreshMainWindow();
	}
	else
		MessageBox(hWnd, L"已是第一个词语。", L"提示", MB_OK);

}

void SenseAlignmentWindow::refreshMainWindow()
{
	// 清除ListView中的所有项 
	ListView_DeleteAllItems(hDictionaryListView_One);
	ListView_DeleteAllItems(hDictionaryListView_Two);

	SetWindowText(hNewRelationshipText, (LPWSTR)L"");	//新映射关系
		
	int row = reader->getCurRowIndex();

	std::string str;
	std::wstring wstr;
	std::vector<std::string> datas;

	//////////////////////////////////////////////////////////////////////
	//词典1	
	LVITEM vitem;
	vitem.mask = LVIF_TEXT;
	vitem.iItem = 0;


	str = reader->getValueInColumnByRow(row, u8"ID");
	wstr = STW_U8(str);
	vitem.iSubItem = 0;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_InsertItem(hDictionaryListView_One, &vitem);
	datas.push_back(str);

	str = reader->getValueInColumnByRow(row, u8"词语");
	wstr = STW_U8(str);
	vitem.iSubItem = 1;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);
	datas.push_back(str);

	str = reader->getValueInColumnByRow(row, u8"义项编码");
	wstr = STW_U8(str);
	vitem.iSubItem = 2;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);
	datas.push_back(str);
	
	str = reader->getValueInColumnByRow(row, u8"拼音");
	wstr = STW_U8(str);
	vitem.iSubItem = 3;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);
	datas.push_back(str);

	str = reader->getValueInColumnByRow(row, u8"词性");
	wstr = STW_U8(str);
	vitem.iSubItem = 4;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);
	datas.push_back(str);

	str = reader->getValueInColumnByRow(row, u8"义项释义");
	wstr = STW_U8(str);
	vitem.iSubItem = 5;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);
	datas.push_back(str);

	str = reader->getValueInColumnByRow(row, u8"示例");
	wstr = STW_U8(str);
	vitem.iSubItem = 6;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);
	datas.push_back(str);
	
	wstr = STW_U8(reader->getValueInColumnByRow(row, u8"相似度"));
	vitem.iSubItem = 7;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);
	
	////////////////////////////////////////////////////////////////////////
	//词典2

	str = reader->getValueInColumnByRow(row, u8"gkb_词语");
	wstr = STW_U8(str);
	vitem.iSubItem = 0;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_InsertItem(hDictionaryListView_Two, &vitem);//区别insert和set
	datas.push_back(str);

	str = reader->getValueInColumnByRow(row, u8"gkb_词类");
	wstr = STW_U8(str);
	vitem.iSubItem = 1;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_Two, &vitem);
	datas.push_back(str);

	str = reader->getValueInColumnByRow(row, u8"gkb_拼音");
	wstr = STW_U8(str);
	vitem.iSubItem = 2;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_Two, &vitem);
	datas.push_back(str);

	str = reader->getValueInColumnByRow(row, u8"gkb_同形");
	wstr = STW_U8(str);
	vitem.iSubItem = 3;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_Two, &vitem);
	datas.push_back(str);

	str = reader->getValueInColumnByRow(row, u8"gkb_释义");
	wstr = STW_U8(str);
	vitem.iSubItem = 4;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_Two, &vitem);
	datas.push_back(str);

	str = reader->getValueInColumnByRow(row, u8"gkb_例句");
	wstr = STW_U8(str);
	vitem.iSubItem = 5;
	vitem.pszText = (LPWSTR)wstr.c_str();
	ListView_SetItem(hDictionaryListView_Two, &vitem);
	datas.push_back(str);
	
	////////////////////////////////////////////////////////////////////////
	wstr = STW_U8(reader->getValueInColumnByRow(row, u8"相似度"));
	SetWindowText(hSimilarityText, (LPWSTR)wstr.c_str());		// 相似度

	str = reader->getValueInColumnByRow(row, u8"映射关系");
	wstr = STW_U8(str);
	//wstr = STW_U8(reader->getValueInColumnByRow(row, u8"映射关系"));
	SetWindowText(hRelationshipText, (LPWSTR)wstr.c_str());		// 映射关系
	datas.push_back(str);

	////////////////////////////////////////////////////////////////////////
	if (recorder->findRecord(datas)) {
		wstr = STW_U8(recorder->getNewMappingRelation());
		SetWindowText(hNewRelationshipText, (LPWSTR)wstr.c_str());	//新映射关系
	}
}

void SenseAlignmentWindow::resetPartOfSpeech()
{
	//获取当前列表框中的选项索引
	int ItemIndex = SendMessage(hClassComboBox, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	//获取当前列表框中的选项值	
	TCHAR  part_of_speech[256];
	(TCHAR)SendMessage(hClassComboBox, (UINT)CB_GETLBTEXT, (WPARAM)ItemIndex, (LPARAM)part_of_speech);

	reader->setPartOfSpeech(WTS_U8(part_of_speech));
}

void SenseAlignmentWindow::recordMappingRelation(const std::string & relation)
{
	int row = reader->getCurRowIndex();

	std::vector<std::string> datas;

	datas.push_back(reader->getValueInColumnByRow(row, u8"ID"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"词语"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"义项编码"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"拼音"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"词性"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"义项释义"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"示例"));

	datas.push_back(reader->getValueInColumnByRow(row, u8"gkb_词语"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"gkb_词类"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"gkb_拼音"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"gkb_同形"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"gkb_释义"));
	datas.push_back(reader->getValueInColumnByRow(row, u8"gkb_例句"));
	
	datas.push_back(reader->getValueInColumnByRow(row, u8"映射关系"));
	datas.push_back(relation);

	if (recorder->findRecord(datas)) {
		recorder->updateMappingRelation(relation);
	}
	else {
		recorder->insertNewRecord(datas);
	}

	std::wstring wstr = STW_U8(relation);
	SetWindowText(hNewRelationshipText, (LPWSTR)wstr.c_str());
}

LRESULT SenseAlignmentWindow::processMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
			if (reader->isExistingFile()) {
				resetPartOfSpeech();
				refreshMainWindow();
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
			if (reader->isExistingFile()) {			
				search();
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

		case ID_BUTTON_EQUAL:
		{
			if (reader->isExistingFile()) {
				recordMappingRelation(u8"等于");
			}
		}
		break;
		case ID_BUTTON_NOT_EQUAL:
		{
			if (reader->isExistingFile()) {
				recordMappingRelation(u8"不等于");
				
			}
		}
		break;
		case ID_BUTTON_BELONG:
		{
			if (reader->isExistingFile()) {
				recordMappingRelation(u8"属于");
				
			}
		}
		break;
		case ID_BUTTON_UNSURE:
		{
			if (reader->isExistingFile()) {
				recordMappingRelation(u8"不确定");
			}
		}
		break;

		case ID_MENUITEM_OPEN:
		{
			openFiles();
			if (reader->isExistingFile()) {
				activeControls(true);

				resetPartOfSpeech();
				refreshMainWindow();
			}
		}
		break;
		case ID_MENUITEM_SAVE:
		{
			recorder->save();
			MessageBox(hWnd, L"已保存。", L"提示", MB_OK);
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
	case WM_SIZE: {
		int nHeight = HIWORD(lParam);
		int nWidth = LOWORD(lParam);

		int heightInc = nHeight - 450;
		int widthInc = nWidth - 800;

		MoveWindow(hDictionaryListView_One, 25, 120, 600 + widthInc, 70, TRUE);
		MoveWindow(hDictionaryListView_Two, 25, 230, 600 + widthInc, 70, TRUE);

		MoveWindow(hRelationEqualButton, 650 + widthInc, 120, 100, 30, TRUE);
		MoveWindow(hRelationNotEqualButton, 650 + widthInc, 170, 100, 30, TRUE);
		MoveWindow(hRelationBelongButton, 650 + widthInc, 220, 100, 30, TRUE);
		MoveWindow(hRelationUnsureButton, 650 + widthInc, 270, 100, 30, TRUE);

		MoveWindow(hSimilarityStaticText, 560 + widthInc, 10, 90, 30, TRUE);
		MoveWindow(hSimilarityText, 650 + widthInc, 10, 100, 30, TRUE);
		MoveWindow(hRelationshipStaticText, 560 + widthInc, 40, 90, 30, TRUE);
		MoveWindow(hRelationshipText, 650 + widthInc, 40, 100, 30, TRUE);
		MoveWindow(hNewRelationshipStaticText, 560 + widthInc, 70, 90, 30, TRUE);
		MoveWindow(hNewRelationshipText, 650 + widthInc, 70, 100, 30, TRUE);

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
	case WM_DESTROY: {
		recorder->save();

		xWindowModule::postQuitMessage();
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK SenseAlignmentWindow::searchEditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
			//Do your stuff
			myWindow->search();
			
			break;  //or return 0; if you don't want to pass it further to def proc
					//If not your key, skip to default:
		}
	default:
		return CallWindowProc(SenseAlignmentWindow::oldSearchEditProc, hWnd, msg, wParam, lParam);
	}
	return 0;
}
