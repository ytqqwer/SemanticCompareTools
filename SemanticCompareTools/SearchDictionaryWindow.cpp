#include "stdafx.h"
#include "SearchDictionaryWindow.h"

#include "SettingModelWindow.h"
#include "WordDetailWindow.h"

#include "WindowClassRegister.h"
#include "xWindowModule.h"

#include "IniFile.h"
#include "sdReader.h"

extern xWindowModule winModule;

// 静态数据成员必须在类定义 外 进行初始化
WNDPROC SearchDictionaryWindow::oldSearchEditProc = NULL;
static SearchDictionaryWindow* myWindow;

SearchDictionaryWindow::SearchDictionaryWindow(HINSTANCE instance) : xWindow(instance)
{
	wcscpy_s(className, L"SearchDictionaryWindow");
	wcscpy_s(title, L"词典查询");

	reader = new sdReader();

	isOpenFile = false;
}

SearchDictionaryWindow::~SearchDictionaryWindow()
{
	delete reader;

	for (auto& wdw: wordDetailWindowsPtr) {
		delete wdw;	
	}
}

void SearchDictionaryWindow::registerClass()
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
	wcex.lpszMenuName = MAKEINTRESOURCEW(ID_MENU_SEARCHDICTIONARY);
	wcex.lpfnWndProc = xWindowModule::WndProc;
	wcex.lpszClassName = className;
	wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

	WindowClassRegister::getInstance()->registerClass(&wcex);
}

void SearchDictionaryWindow::initWindow()
{
	myWindow = this;

	winModule.AddXWindowObject(nullptr, this);

	hWnd = CreateWindowW(className, title, WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX,
		CW_USEDEFAULT, 0, 800, 450, nullptr, nullptr, hInstance, nullptr);

	//////////////////////////////////////////////////////////////////////
	//初始化搜索编辑框
	hSearchEdit = CreateWindow(_T("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT,
		70, 20, 200, 30, hWnd, (HMENU)ID_EDIT_SEARCH, hInstance, NULL);
	//设置处理过程
	oldSearchEditProc = (WNDPROC)SetWindowLongPtr(hSearchEdit, GWLP_WNDPROC, (LONG_PTR)SearchDictionaryWindow::searchEditProc);

	//////////////////////////////////////////////////////////////////////
	//初始化搜索按钮
	hSearchButton = CreateWindow(_T("BUTTON"), _T("搜索"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		310, 20, 100, 30, hWnd, (HMENU)ID_BUTTON_SEARCH, hInstance, NULL);

	//////////////////////////////////////////////////////////////////////
	//初始化文本	
	hWordText = CreateWindow(_T("static"), _T("词语"), WS_CHILD | WS_VISIBLE | SS_LEFT, 30, 25, 30, 30, hWnd,
		NULL, hInstance, NULL);

	//////////////////////////////////////////////////////////////////////
	//初始化文件名下拉列表
	hFileNameOrPartOfSpeechComboBox = CreateWindow(WC_COMBOBOX, _T(""), CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		440, 20, 160, 800, hWnd, (HMENU)ID_COMBOBOX_FILENAME, hInstance, NULL);

	//////////////////////////////////////////////////////////////////////
	//初始化词典的列表视图
	//无需自动加载词语信息并显示
	hDictionaryListView = CreateWindowW(WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_NOSORTHEADER,
		25, 65, 740, 300, hWnd, (HMENU)ID_DICTIONARY_LISTVIEW, hInstance, NULL);

	ListView_SetExtendedListViewStyle(hDictionaryListView, LVS_EX_FULLROWSELECT);     //设置整行选择风格
	
	//////////////////////////////////////////////////////////////////////
	//初始化词典名
	hDicNameText = CreateWindow(_T("static"), _T("词典"), WS_CHILD | WS_VISIBLE | SS_LEFT, 650, 25, 200, 30, hWnd,
		NULL, hInstance, NULL);
	
	//////////////////////////////////////////////////////////////////////
	//设置字体
	HFONT hFont = CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"微软雅黑");//创建字体
	SendMessage(hSearchButton, WM_SETFONT, (WPARAM)hFont, TRUE);//发送设置字体消息
	SendMessage(hSearchEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hWordText, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hFileNameOrPartOfSpeechComboBox, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hDicNameText, WM_SETFONT, (WPARAM)GetStockObject(17), 0);

	//////////////////////////////////////////////////////////////////////
	// 禁用某些功能，直到被激活，TRUE使用，FALSE禁止
	activeControls(FALSE);

}

BOOL SearchDictionaryWindow::activeControls(bool boolean)
{
	EnableWindow(hSearchButton, boolean);
	EnableWindow(hSearchEdit, boolean);
	EnableWindow(hFileNameOrPartOfSpeechComboBox, boolean);
	EnableWindow(hDictionaryListView, boolean);

	return 0;
}

LRESULT SearchDictionaryWindow::processMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{

	case WM_NOTIFY:
	{
		switch (wParam)
		{
		case ID_DICTIONARY_LISTVIEW:
		{
			LPNMITEMACTIVATE activeItem = (LPNMITEMACTIVATE)lParam; //得到NMITEMACTIVATE结构指针  
			if (activeItem->iItem >= 0)				//当点击空白区域时，iItem值为-1，防止越界
			{
				switch (activeItem->hdr.code) {		//判断通知码  
				case NM_CLICK: //单击
				{
					//do nothing

				}	break;
				case NM_DBLCLK: //双击
				{
					//char number[50];
					//_itoa_s(activeItem->iItem, number, 10);	//activeItem->iItem项目序号，将int整型数转化为一个字符串
					//MessageBoxA(hWnd, number, "双击", 0);
					
					WordDetailWindow* smw = new WordDetailWindow(hInstance);

					//有序组织数据
					auto& subItems = listViewRows[activeItem->iItem].subItems;
					auto& columnNames = reader->getColumnNames(listViewRows[activeItem->iItem].fileName);

					for (auto& columnName : columnNames) {
						auto& content = subItems.at(columnName);
						smw->pushData(std::make_pair(columnName, content));
					}

					//创建窗口
					wordDetailWindowsPtr.push_back(smw);
					winModule.createWindow(smw);

				}	break;
				case NM_RCLICK: //右击
				{
					WordDetailWindow* smw = new WordDetailWindow(hInstance);

					//有序组织数据
					auto& subItems = listViewRows[activeItem->iItem].subItems;
					auto& columnNames = reader->getColumnNames(listViewRows[activeItem->iItem].fileName);

					for (auto& columnName : columnNames) {
						auto& content = subItems.at(columnName);
						smw->pushData(std::make_pair(columnName, content));
					}

					//创建窗口
					wordDetailWindowsPtr.push_back(smw);
					winModule.createWindow(smw);

				}   break;
				}
			}

		}break;
		}
	}break;

	case WM_COMMAND:
	{
		// 分析wParam高位: 
		switch (HIWORD(wParam))
		{
		case CBN_SELCHANGE://列表框选择发生变化
		{
			//do nothing
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
		case ID_MENUITEM_SETTING:
		{
			if (isOpenFile) {
				SettingModelWindow* smw = new SettingModelWindow(hInstance, hWnd);
				smw->setParentWindowPtr(this);
				winModule.createModelWindow(smw, hWnd);

				//重设文件名与词类对应关系，刷新列表
				resetFNAPOS();

				resetDicName();

				refreshComboBox();

				reader->setSelectedColumnNamesAndWordColumnName();
			}
			else {
				MessageBox(hWnd, L"请先打开有效的文件。", L"提示", MB_OK);
			}

		}
		break;
		case ID_MENUITEM_OPEN:
		{
			openFiles();

			if (isOpenFile) {			
				activeControls(TRUE);

				resetFNAPOS();

				resetDicName();

				refreshComboBox();

				reader->setSelectedColumnNamesAndWordColumnName();


				SettingModelWindow* smw = new SettingModelWindow(hInstance, hWnd);
				smw->setParentWindowPtr(this);
				winModule.createModelWindow(smw, hWnd);
			}
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
	case WM_SIZE:{
		int nHeight = HIWORD(lParam);
		int nWidth = LOWORD(lParam);

		int heightInc = nHeight - 450;
		int widthInc = nWidth - 800;

		MoveWindow(hDictionaryListView, 25, 65, 740 + widthInc, 350+ heightInc, TRUE);
	}
	break;
	case WM_GETMINMAXINFO:{
		MINMAXINFO * pMinMaxInfo = (MINMAXINFO *)lParam;
		pMinMaxInfo -> ptMinTrackSize.x = 800;
		pMinMaxInfo -> ptMinTrackSize.y = 450;
		pMinMaxInfo	-> ptMaxTrackSize.x = 1600;
		pMinMaxInfo	-> ptMaxTrackSize.y = 900;

		return 0;
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
	{
		xWindowModule::postQuitMessage();
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT SearchDictionaryWindow::searchEditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
			case VK_RETURN:
			{
				myWindow->search();
			}
			
			break;
		}
	default:
		return CallWindowProc(SearchDictionaryWindow::oldSearchEditProc, hWnd, msg, wParam, lParam);
	}
	return 0;
}

void SearchDictionaryWindow::search()
{
	listViewRows.clear();

	///////////////////////////////////////////////////////////////////////////
	TCHAR buff[80] = _T("");
	GetWindowText(hSearchEdit, buff, 80);
	const std::string& word = WTS_U8(buff);

	//CB_GETCURSEL - 获取当前被选择项。
	//	- WPARAM / LPARAM 均未使用，必须为0
	//	- SendMessage函数的返回，获取选择项的索引，如果当前没有选择项返回CB_ERR
	int index = SendMessage(hFileNameOrPartOfSpeechComboBox, CB_GETCURSEL, 0, 0);

	if (index == 0) {
		//首项说明是全部搜索
		reader->findWord(word, listViewRows);
	}
	else {	
		//CB_GETLBTEXT - 获取选项字符的内容
		//	- WPARAM - 选项的索引
		//	- LPARAM - 获取字符内容的BUFF
		TCHAR temp[200];
		SendMessage(hFileNameOrPartOfSpeechComboBox, CB_GETLBTEXT, index, (LPARAM)temp);
		std::wstring wstr = temp;
		std::string str = WTS_U8(wstr);

		//判断是文件名还是词类名
		bool isFileName = false;

		for (auto& fnapos : FNAPOS) 
		{
			if (fnapos.fileName == str) {
				isFileName = true;
				break;
			}
		}

		if (isFileName) {	
			//是文件名，只需搜索一个文件
			reader->findWord(str, word, listViewRows);
		}
		else {
			//是词类，需搜索每个相同词类对应的文件
			for (auto& fnapos : FNAPOS)
			{
				if (fnapos.partOfSpeech == str) {
					reader->findWord(fnapos.fileName, word, listViewRows);
				}
			}
		
		}
	
	}

	///////////////////////////////////////////////////////////////////////////
	if (listViewRows.size()) {
		refreshListView();
	}
	else {
		MessageBox(hWnd, L"未找到该词语。", L"提示", MB_OK);
	}

}

void SearchDictionaryWindow::resetFNAPOS()
{
	TCHAR temp[100];
	std::vector<std::string>& fileNames = reader->getFileNames();
	FNAPOS.clear();
	for (auto& fileName : fileNames)
	{
		FileNameAndPartOfSpeech fnapos;
		fnapos.fileName = fileName;

		std::wstring wFileName =  STW_U8(fileName);
		IniFile::getInstance()->GetProfileStringWithPath(wFileName.c_str(), L"PartOfSpeech", temp);
		
		std::wstring wstr = temp;
		std::string partOfSpeech = WTS_U8(wstr);
		fnapos.partOfSpeech = partOfSpeech;

		FNAPOS.push_back(fnapos);
	}
}

void SearchDictionaryWindow::resetDicName()
{
	TCHAR temp[80];	
	IniFile::getInstance()->GetProfileStringWithPath(L"", L"CustomDicName", temp);
	SetWindowText(hDicNameText,temp);
}

void SearchDictionaryWindow::refreshComboBox()
{
	//重置内容
	SendMessage(hFileNameOrPartOfSpeechComboBox, CB_RESETCONTENT, 0, 0);

	//全部搜索列表项
	SendMessage(hFileNameOrPartOfSpeechComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"全部");

	TCHAR temp[100];
	std::vector<std::string>& fileNames = reader->getFileNames();

	//更新下拉列表内容
	for (auto& fnapos : FNAPOS)
	{
		if (fnapos.partOfSpeech != "")
		{
			auto c_str = fnapos.partOfSpeech.c_str();
			CharToTchar(c_str, temp);
			SendMessage(hFileNameOrPartOfSpeechComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)temp);
		}
		else {
			auto c_str = fnapos.fileName.c_str();
			CharToTchar(c_str, temp);
			SendMessage(hFileNameOrPartOfSpeechComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)temp);
		}
	}

	// Send the CB_SETCURSEL message to display an initial item in the selection field  
	SendMessage(hFileNameOrPartOfSpeechComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
}

void SearchDictionaryWindow::refreshListView()
{
	// 清除ListView中的所有项 
	// 删除所有行
	//ListView_DeleteAllItems(hDictionaryListView);
	SendMessage(hDictionaryListView, LVM_DELETEALLITEMS, 0, 0);

	//得到ListView的Header窗体
	HWND hWndListViewHeader = (HWND)SendMessage(hDictionaryListView, LVM_GETHEADER, 0, 0);
	//得到列的数目
	int nCols = SendMessage(hWndListViewHeader, HDM_GETITEMCOUNT, 0, 0);
	//删除所有列	
	nCols--;
	for (; nCols >= 0; nCols--)
		SendMessage(hDictionaryListView, LVM_DELETECOLUMN, nCols, 0);
		
	//////////////////////////////////////////////////////////
	//插入数据以及补充列名

	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;

	LVITEM vitem;
	vitem.mask = LVIF_TEXT;

	std::unordered_map<std::string, int> columnNameMap;

	int columnNameCount = 0;
	int rowIndex = 0;

	bool vitemFirstInsert = true;

	for (auto& lvr : listViewRows)
	{
		auto& columnNames = reader->getDisplayColumnNames(lvr.fileName);

		vitem.iItem = rowIndex;

		//检查与添加列名
		for (auto& columnName : columnNames)
		{
			auto& it = columnNameMap.find(columnName);
			if (it == columnNameMap.end()) //插入新的列名
			{
				columnNameMap.insert(std::make_pair(columnName, columnNameCount));

				lvc.iSubItem = columnNameCount;
				columnNameCount++;

				//列名
				std::wstring wColumnName = STW_U8(columnName);
				lvc.pszText = (LPWSTR)wColumnName.c_str();  //强制转换

				//此新列对应内容
				std::wstring content = L"";
				auto it = lvr.subItems.find(columnName);
				if (it != lvr.subItems.end()) {
					content = STW_U8((*it).second);
				}

				//统计宽度，决定列所占像素
				unsigned int columnNameWidth = CalWstringWidth(wColumnName);
				unsigned int contentWidth = CalWstringWidth(content);
				unsigned int width = columnNameWidth < contentWidth ? contentWidth : columnNameWidth;
				width = width <= 0 ? 50 : (width > 400 ? 400 : width);

				lvc.cx = width;

				// Insert the columns into the list view.
				if (ListView_InsertColumn(hDictionaryListView, lvc.iSubItem, &lvc) == -1)
					return;
			}

			vitem.iSubItem = columnNameMap.at(columnName);

			auto& iter = lvr.subItems.find(columnName);
			if (iter != lvr.subItems.end()) {


				std::wstring content = STW_U8((*iter).second);
				vitem.pszText = (LPWSTR)content.c_str();

				if (vitemFirstInsert) {
					ListView_InsertItem(hDictionaryListView, &vitem);
					vitemFirstInsert = false;
				}
				else {
					ListView_SetItem(hDictionaryListView, &vitem);
				}

			}

		}
		vitemFirstInsert = true;
		rowIndex++;

	}
}

void SearchDictionaryWindow::openFiles()
{
	//调用 shell32.dll api
	//调用浏览文件夹对话框
	TCHAR szPathName[MAX_PATH];
	BROWSEINFO bInfo = { 0 };
	bInfo.hwndOwner = GetForegroundWindow();//父窗口  
	bInfo.lpszTitle = TEXT("选择文件夹");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI/*包含一个编辑框 用户可以手动填写路径 对话框可以调整大小之类的..*/ |
		BIF_UAHINT/*带TIPS提示*/ | BIF_NONEWFOLDERBUTTON /*不带新建文件夹按钮*/;
	LPITEMIDLIST lpDlist = SHBrowseForFolder(&bInfo);
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
		//搜索目录下的xlsx文件，并记录文件名
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

				reader->addXlsxFile(utf8_str, WTS_U8(wpath));

			} while (_findnext(lfDir, &fileDir) == 0);
		}
		_findclose(lfDir);

		isOpenFile = true;
	}

}