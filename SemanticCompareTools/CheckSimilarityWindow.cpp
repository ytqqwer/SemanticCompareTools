#include "stdafx.h"
#include "CheckSimilarityWindow.h"

#include "windowClassRegister.h"
#include "xWindowModule.h"

#include "IniFile.h"
#include "csReader.h"
#include "csRecorder.h"

#include <set>

extern xWindowModule winModule;

// ��̬���ݳ�Ա�������ඨ�� �� ���г�ʼ��
WNDPROC CheckSimilarityWindow::oldSearchEditProc = NULL;
static CheckSimilarityWindow* myWindow;


CheckSimilarityWindow::CheckSimilarityWindow(HINSTANCE instance) : xWindow(instance)
{
	wcscpy_s(className, L"CheckSimilarityWindow");
	wcscpy_s(title, L"����Ա�");
		
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
		u8"gkb_����",
		u8"gkb_����",
		u8"gkb_ƴ��",
		u8"gkb_ͬ��",
		u8"gkb_����",
		u8"gkb_����",
		u8"ID",
		u8"����",
		u8"�������",
		u8"ƴ��",
		u8"����",
		u8"��������",
		u8"ʾ��",
		u8"���ƶ�"
	};
	reader->setColumnNames(columnNames);
	reader->setWordColumnName(u8"gkb_����");

	//////////////////////////////////////////////////////////////////////
	//��ʼ�������༭��
	myWindow = this;

	hSearchEdit = CreateWindow(_T("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT,
		70, 10, 200, 30, hWnd, (HMENU)ID_EDIT_SEARCH, hInstance, NULL);
	//���ô������
	CheckSimilarityWindow::oldSearchEditProc = 
		(WNDPROC)SetWindowLongPtr(hSearchEdit, GWLP_WNDPROC, (LONG_PTR)CheckSimilarityWindow::searchEditProc);

	//////////////////////////////////////////////////////////////////////
	//��ʼ��������ť
	hSearchButton = CreateWindow(_T("BUTTON"), _T("����"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		310, 10, 100, 30, hWnd, (HMENU)ID_BUTTON_SEARCH, hInstance, NULL);

	//////////////////////////////////////////////////////////////////////
	//��ʼ����������б�
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

	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"δ");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"a");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"u");
	SendMessage(hClassComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"׺");

	// Send the CB_SETCURSEL message to display an initial item in the selection field  
	SendMessage(hClassComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	//////////////////////////////////////////////////////////////////////
	//��ʼ���ı�	
	hWordText = CreateWindow(_T("static"), _T("����"), WS_CHILD | WS_VISIBLE | SS_LEFT, 30, 15, 30, 30, hWnd,
		NULL, hInstance, NULL);

	hPosText = CreateWindow(_T("static"), _T("ѡ�����"), WS_CHILD | WS_VISIBLE | SS_LEFT, 500, 15, 80, 30, hWnd,
		NULL, hInstance, NULL);

	hDicText_One = CreateWindow(_T("static"), _T("GKB�ִ������﷨��Ϣ�ʵ�"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		30, 47, 300, 30, hWnd,	NULL, hInstance, NULL);

	hDicText_Two = CreateWindow(_T("static"), _T("�ִ�����ʵ�"), WS_CHILD | WS_VISIBLE | SS_LEFT,
		30, 135, 300, 30, hWnd,	NULL, hInstance, NULL);

	HFONT hFont = CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"΢���ź�");//��������
	SendMessage(hSearchButton, WM_SETFONT, (WPARAM)hFont, TRUE);//��������������Ϣ
	SendMessage(hSearchEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hWordText, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hPosText, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hDicText_One, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessage(hDicText_Two, WM_SETFONT, (WPARAM)hFont, TRUE);
	
	{
		//////////////////////////////////////////////////////////////////////
		//��ʼ���ʵ�1���б���ͼ
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
		wcscpy_s(szText, L"����");
		if (ListView_InsertColumn(hDictionaryListView_One, 0, &lvc) == -1)
			return;
		
		lvc.iSubItem = 1;
		lvc.pszText = szText;
		lvc.cx = 40;
		wcscpy_s(szText, L"����");
		if (ListView_InsertColumn(hDictionaryListView_One, 1, &lvc) == -1)
			return;
		
		lvc.iSubItem = 2;
		lvc.pszText = szText;
		lvc.cx = 100;
		wcscpy_s(szText, L"ƴ��");
		if (ListView_InsertColumn(hDictionaryListView_One, 2, &lvc) == -1)
			return;
		
		lvc.iSubItem = 3;
		lvc.pszText = szText;
		lvc.cx = 40;
		wcscpy_s(szText, L"ͬ��");
		if (ListView_InsertColumn(hDictionaryListView_One, 3, &lvc) == -1)
			return;
		
		lvc.iSubItem = 4;
		lvc.pszText = szText;
		lvc.cx = 245;
		wcscpy_s(szText, L"����");
		if (ListView_InsertColumn(hDictionaryListView_One, 4, &lvc) == -1)
			return;
		
		lvc.iSubItem = 5;
		lvc.pszText = szText;
		lvc.cx = 255;
		wcscpy_s(szText, L"����");
		if (ListView_InsertColumn(hDictionaryListView_One, 5, &lvc) == -1)
			return;
			
		//////////////////////////////////////////////////////////////////////
		//��ʼ���ʵ�2���б���ͼ
		hDictionaryListView_Two = CreateWindowW(WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | WS_BORDER |
			LVS_REPORT | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS,
			25, 160, 740, 180, hWnd, (HMENU)ID_LISTVIEW_DICTIONARY_TWO, hInstance, NULL);

		ListView_SetExtendedListViewStyle(hDictionaryListView_Two, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);      //��������ѡ����
		
		lvc.iSubItem = 0;
		lvc.pszText = szText;
		lvc.cx = 70;
		wcscpy_s(szText, L"ID");
		if (ListView_InsertColumn(hDictionaryListView_Two, 0, &lvc) == -1)
			return;

		lvc.iSubItem = 1;
		lvc.pszText = szText;
		lvc.cx = 60;
		wcscpy_s(szText, L"����");
		if (ListView_InsertColumn(hDictionaryListView_Two, 1, &lvc) == -1)
			return;

		lvc.iSubItem = 2;
		lvc.pszText = szText;
		lvc.cx = 90;
		wcscpy_s(szText, L"�������");
		if (ListView_InsertColumn(hDictionaryListView_Two, 2, &lvc) == -1)
			return;

		lvc.iSubItem = 3;
		lvc.pszText = szText;
		lvc.cx = 75;
		wcscpy_s(szText, L"ƴ��");
		if (ListView_InsertColumn(hDictionaryListView_Two, 3, &lvc) == -1)
			return;
		
		lvc.iSubItem = 4;
		lvc.pszText = szText;
		lvc.cx = 40;
		wcscpy_s(szText, L"����");
		if (ListView_InsertColumn(hDictionaryListView_Two, 4, &lvc) == -1)
			return;
		
		lvc.iSubItem = 5;
		lvc.pszText = szText;
		lvc.cx = 170;
		wcscpy_s(szText, L"��������");
		if (ListView_InsertColumn(hDictionaryListView_Two, 5, &lvc) == -1)
			return;

		lvc.iSubItem = 6;
		lvc.pszText = szText;
		lvc.cx = 180;
		wcscpy_s(szText, L"ʾ��");
		if (ListView_InsertColumn(hDictionaryListView_Two, 6, &lvc) == -1)
			return;

		lvc.iSubItem = 7;
		lvc.pszText = szText;
		lvc.cx = 50;
		wcscpy_s(szText, L"���ƶ�");
		if (ListView_InsertColumn(hDictionaryListView_Two, 7, &lvc) == -1)
			return;

	}

	//////////////////////////////////////////////////////////////////////
	//Check��ť
	hCheckButton = CreateWindow(_T("BUTTON"), _T("Check"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		60, 350, 100, 30, hWnd, (HMENU)ID_BUTTON_CHECK, hInstance, NULL);

	//////////////////////////////////////////////////////////////////////
	//����һ����ť
	hPrevWordButton = CreateWindow(_T("BUTTON"), _T("��һ��"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		250, 350, 100, 30, hWnd, (HMENU)ID_BUTTON_PREV_WORD, hInstance, NULL);

	//////////////////////////////////////////////////////////////////////
	//����һ����ť
	hNextWordButton = CreateWindow(_T("BUTTON"), _T("��һ��"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		450, 350, 100, 30, hWnd, (HMENU)ID_BUTTON_NEXT_WORD, hInstance, NULL);
	
	//////////////////////////////////////////////////////////////////////
	// ����ĳЩ���ܣ�ֱ�������TRUEʹ�ã�FALSE��ֹ
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
		MessageBox(hWnd, L"�ڵ�ǰ������δ�ҵ��ô��", L"��ʾ", MB_OK);
	}

}

void CheckSimilarityWindow::openFiles()
{
	//���� shell32.dll api   ��������ļ��жԻ��� 
	TCHAR szPathName[MAX_PATH];
	BROWSEINFO bInfo = { 0 };
	bInfo.hwndOwner = GetForegroundWindow();//������  
	bInfo.lpszTitle = TEXT("��ѡ������Ӧ�����ڵ��ļ���");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI/*����һ���༭�� �û������ֶ���д·�� �Ի�����Ե�����С֮���..*/ |
		BIF_UAHINT/*��TIPS��ʾ*/ | BIF_NONEWFOLDERBUTTON /*�����½��ļ��а�ť*/;
	LPITEMIDLIST lpDlist;
	lpDlist = SHBrowseForFolder(&bInfo);
	if (lpDlist != NULL)//������ȷ����ť  
	{
		////////////////////////////////////////////////////////////////////
		// ��ȡĿ¼��Ϊ��֧�����ģ���Ҫ����ȡ���ַ�����TCHAR����תΪCHAR���룬ʹ�ô���ҳCP_ACP
		SHGetPathFromIDList(lpDlist, szPathName);

		char cPath[256];
		//��ȡ�ֽڳ���   
		int iLength = WideCharToMultiByte(CP_ACP, 0, szPathName, -1, NULL, 0, NULL, NULL);
		//��tcharֵ����_char    
		WideCharToMultiByte(CP_ACP, 0, szPathName, -1, cPath, iLength, NULL, NULL);
		
		////////////////////////////////////////////////////////////////////
		IniFile::getInstance()->SetFilePath(szPathName);

		szFilePath = szPathName;

		////////////////////////////////////////////////////////////////////
		//����Ŀ¼�µ�xlsx�ļ�
		std::string searchPath = cPath;
		searchPath = searchPath + "\\*.xlsx";

		std::wstring wpath = szPathName;
		wpath = wpath + L"\\";

		_finddata_t fileDir;
		long lfDir;
		if ((lfDir = _findfirst(searchPath.c_str(), &fileDir)) == -1l) {
			MessageBox(hWnd, L"No xlsx file is found\n", L"��ʾ", MB_OK);
			return;
		}
		else {
			//�������֮ǰ�����������Ϣ
			reader->clear();

			do {
				//�ҵ���xlsx�ļ����Ƚ�charת��Ϊwchar���ٽ�wcharת��Ϊutf-8�����char
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
	//����������ʽ���Լ�����Ĵ���Ӣ����ĸ
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
		reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_DAI), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_DONG), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_FU), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_JIE), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_LIAN), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_LIANG), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_MING), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_NI), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_SHU), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"̾" + pattern_end), WTS_U8(PART_OF_SPEECH_TAN), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"δ" + pattern_end), WTS_U8(PART_OF_SPEECH_WEI), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_XING), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"��" + pattern_end), WTS_U8(PART_OF_SPEECH_ZHU), WTS_U8(wpath));
		reader->loadXlsxFile(WTS_U8(part + L"׺" + pattern_end), WTS_U8(PART_OF_SPEECH_ZHUI), WTS_U8(wpath));
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
		MessageBox(hWnd, L"�������һ�����", L"��ʾ", MB_OK);
}

void CheckSimilarityWindow::prev()
{
	if (reader->toPrevIsomorphicOfCurWord() || reader->prevWord()) {	//��·��ֵ
		refreshMainWindow();
	}
	else
		MessageBox(hWnd, L"���ǵ�һ�����", L"��ʾ", MB_OK);
}

void CheckSimilarityWindow::refreshListView()
{
	// ���ListView�е������� 
	ListView_DeleteAllItems(hDictionaryListView_One);
	ListView_DeleteAllItems(hDictionaryListView_Two);

	std::pair<unsigned int, std::vector<unsigned int>> neededRows = reader->getRowsByIsomorphicIndex();

	rowOfWord = neededRows.first;

	LVITEM vitem;
	vitem.mask = LVIF_TEXT;
	vitem.iItem = 0;

	std::wstring str;//��ʱ����str���淵�صĿ��ַ��ַ���	
					 //////////////////////////////////////////////////////////////////////
					 //�ʵ�1	
	str = stringToWstring(reader->getValueInColumnByRow(neededRows.first, u8"gkb_����"));
	vitem.iSubItem = 0;
	vitem.pszText = (LPWSTR)str.c_str();
	ListView_InsertItem(hDictionaryListView_One, &vitem);//����insert��set

	str = stringToWstring(reader->getValueInColumnByRow(neededRows.first, u8"gkb_����"));
	vitem.iSubItem = 1;
	vitem.pszText = (LPWSTR)str.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);

	str = stringToWstring(reader->getValueInColumnByRow(neededRows.first, u8"gkb_ƴ��"));
	vitem.iSubItem = 2;
	vitem.pszText = (LPWSTR)str.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);

	str = stringToWstring(reader->getValueInColumnByRow(neededRows.first, u8"gkb_ͬ��"));
	vitem.iSubItem = 3;
	vitem.pszText = (LPWSTR)str.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);

	str = stringToWstring(reader->getValueInColumnByRow(neededRows.first, u8"gkb_����"));
	vitem.iSubItem = 4;
	vitem.pszText = (LPWSTR)str.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);

	str = stringToWstring(reader->getValueInColumnByRow(neededRows.first, u8"gkb_����"));
	vitem.iSubItem = 5;
	vitem.pszText = (LPWSTR)str.c_str();
	ListView_SetItem(hDictionaryListView_One, &vitem);

	////////////////////////////////////////////////////////////////////////
	//�ʵ�2
	numberOfItem = neededRows.second.size();

	for (int i = 0; i < numberOfItem; i++)
	{
		unsigned int row = neededRows.second[i];

		vitem.iItem = i;

		str = stringToWstring(reader->getValueInColumnByRow(row, u8"ID"));
		vitem.iSubItem = 0;
		vitem.pszText = (LPWSTR)str.c_str();
		ListView_InsertItem(hDictionaryListView_Two, &vitem);

		str = stringToWstring(reader->getValueInColumnByRow(row, u8"����"));
		vitem.iSubItem = 1;
		vitem.pszText = (LPWSTR)str.c_str();
		ListView_SetItem(hDictionaryListView_Two, &vitem);

		str = stringToWstring(reader->getValueInColumnByRow(row, u8"�������"));
		vitem.iSubItem = 2;
		vitem.pszText = (LPWSTR)str.c_str();
		ListView_SetItem(hDictionaryListView_Two, &vitem);

		str = stringToWstring(reader->getValueInColumnByRow(row, u8"ƴ��"));
		vitem.iSubItem = 3;
		vitem.pszText = (LPWSTR)str.c_str();
		ListView_SetItem(hDictionaryListView_Two, &vitem);

		str = stringToWstring(reader->getValueInColumnByRow(row, u8"����"));
		vitem.iSubItem = 4;
		vitem.pszText = (LPWSTR)str.c_str();
		ListView_SetItem(hDictionaryListView_Two, &vitem);

		str = stringToWstring(reader->getValueInColumnByRow(row, u8"��������"));
		vitem.iSubItem = 5;
		vitem.pszText = (LPWSTR)str.c_str();
		ListView_SetItem(hDictionaryListView_Two, &vitem);

		str = stringToWstring(reader->getValueInColumnByRow(row, u8"ʾ��"));
		vitem.iSubItem = 6;
		vitem.pszText = (LPWSTR)str.c_str();
		ListView_SetItem(hDictionaryListView_Two, &vitem);

		str = stringToWstring(reader->getValueInColumnByRow(row, u8"���ƶ�"));
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
	//��ȡ��ǰ�б���е�ѡ������
	int ItemIndex = SendMessage(hClassComboBox, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	//��ȡ��ǰ�б���е�ѡ��ֵ	
	TCHAR  part_of_speech[256];
	(TCHAR)SendMessage(hClassComboBox, (UINT)CB_GETLBTEXT, (WPARAM)ItemIndex, (LPARAM)part_of_speech);

	reader->setPartOfSpeech(WTS_U8(part_of_speech));

}

void CheckSimilarityWindow::insertAllRecord()
{

	do
	{
		std::pair<unsigned int, std::vector<unsigned int>>& neededRows = reader->getRowsByIsomorphicIndex();
		
		std::string& word = reader->getValueInColumnByRow(neededRows.first, u8"gkb_����");
		std::string& pos = reader->getValueInColumnByRow(neededRows.first, u8"gkb_����");
		std::string& alphabetic = reader->getValueInColumnByRow(neededRows.first, u8"gkb_ƴ��");
		std::string& isomorphic = reader->getValueInColumnByRow(neededRows.first, u8"gkb_ͬ��");
		std::string& meanings = reader->getValueInColumnByRow(neededRows.first, u8"gkb_����");
		std::string& example = reader->getValueInColumnByRow(neededRows.first, u8"gkb_����");

		//�Ӽ�¼��Ѱ���Ƿ������ȫ���ϵĴ���
		if (recorder->findRecord(word, pos, alphabetic, isomorphic, meanings, example))
		{
			continue;
		}
		//û���ҵ���ͬ��¼���Զ��ж����ƶȴ�С��ϵ������¼�¼
		else
		{
			std::vector<std::pair<std::string,double>> ids_sims;
			for (auto& row : neededRows.second) 
			{
				std::string& id = reader->getValueInColumnByRow(row, u8"ID");
				std::string& sim = reader->getValueInColumnByRow(row, u8"���ƶ�");

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

			//�����µļ�¼
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
	//��ȡ��ǰGKB�б��еĴ�����Ϣ
	TCHAR buf[256];
	ListView_GetItemText(hDictionaryListView_One, 0, 0, buf, 256);//�����item��iSubItem�����飬ѡȡ�Ĵ�С
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

	//�Ӽ�¼��Ѱ���Ƿ������ȫ���ϵĴ���
	if (recorder->findRecord(word, pos, alphabetic, isomorphic, meanings, example))
	{
		std::vector<std::string> IDs = recorder->getIDsAfterFind();

		for (auto& id : IDs)
		{
			for (int i = 0; i < numberOfItem; i++)
			{
				//ȡID
				ListView_GetItemText(hDictionaryListView_Two, i, 0, buf, 256);

				if (id == WTS_U8(buf)) {
					ListView_SetCheckState(hDictionaryListView_Two, i, TRUE);
					break;
				}

			}
		}

	}

	//û���ҵ���ͬ��¼���Զ��ж����ƶȴ�С��ϵ���Զ��򹴣�ȡ�򹴽��������¼�¼
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

	//	//ȡ�׸����ƶ�
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
	//	//�洢ѡȡ��id
	//	std::vector<std::string> ids;
	//	for (unsigned int i = 0; i < numberOfItem; i++)
	//	{
	//		if (ListView_GetCheckState(hDictionaryListView_Two, i)) {

	//			TCHAR buffer[256];
	//			ListView_GetItemText(hDictionaryListView_Two, i, 0, buffer, 256);

	//			ids.push_back(WTS_U8(buffer));
	//		}
	//	}

	//	//�����µļ�¼
	//	std::vector<std::string> vector;
	//	std::string str;
	//	str = reader->getValueInColumnByRow(rowOfWord, u8"gkb_����");
	//	vector.push_back(str);
	//	str = reader->getValueInColumnByRow(rowOfWord, u8"gkb_����");
	//	vector.push_back(str);
	//	str = reader->getValueInColumnByRow(rowOfWord, u8"gkb_ƴ��");
	//	vector.push_back(str);
	//	str = reader->getValueInColumnByRow(rowOfWord, u8"gkb_ͬ��");
	//	vector.push_back(str);
	//	str = reader->getValueInColumnByRow(rowOfWord, u8"gkb_����");
	//	vector.push_back(str);
	//	str = reader->getValueInColumnByRow(rowOfWord, u8"gkb_����");
	//	vector.push_back(str);

	//	recorder->insertNewRecord(vector, ids);
	//}


}

void CheckSimilarityWindow::check()
{
	//�洢ѡȡ��id��ʹ��string���ͼ�¼����ȻЧ�ʻ��ܶ�
	std::vector<std::string> ids;
	for (int i = 0; i < numberOfItem; i++)
	{
		if (ListView_GetCheckState(hDictionaryListView_Two, i)) {

			TCHAR buf[256];
			ListView_GetItemText(hDictionaryListView_Two, i, 0, buf, 256);

			ids.push_back(WTS_U8(buf));
			//MessageBox(hWnd, buf, L"��ʾ", MB_OK);
		}
	}

	//��ȡ��ǰGKB�б��еĴ�����Ϣ
	TCHAR buf[256];
	ListView_GetItemText(hDictionaryListView_One, 0, 0, buf, 256);//�����item��iSubItem�����飬ѡȡ�Ĵ�С
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

	//�Ӽ�¼��Ѱ���Ƿ������ȫ���ϵĴ���
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
		// ����wParam��λ: 
		switch (HIWORD(wParam))
		{
		case CBN_SELCHANGE:
		{
			//ֻ�е���ȡ���Ѿ����ع��ļ�ʱ�����Զ�ˢ��������
			//��ʱ����Ҫ�滻���࣬���ļ�ʱҲ������б�������ѡ��һ�δ���
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
			//��������
			break;
		}

		// ����wParam��λ: 
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
				// ����ĳЩ�ؼ�
				activeControls(TRUE);

				// ���ļ���Ĭ����ʾ��ǰ����ѡ���б���ָ���ı��ˢ��������
				// ������δ���ļ�ʱ��Ҳ�������ѡ����࣬���ǲ������Ч��
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
			MessageBox(hWnd, L"�ѱ��档", L"��ʾ", MB_OK);
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
		// TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
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
